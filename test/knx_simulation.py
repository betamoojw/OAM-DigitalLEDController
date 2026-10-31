"""Hardware-free simulation of the DigitalLedModule KNX input boundary."""

from __future__ import annotations

import json
from dataclasses import dataclass, field


KO_DIM_SW = 201
KO_DIM_REL = 202
KO_DIM_ABS = 203
KO_SCENE = 206
KO_RED = 208
KO_GREEN = 210
KO_BLUE = 212
KO_RGB = 214
KO_NIGHTLIGHT = 216
KO_PRESET = 218
KO_PALETTE = 220
KO_EFFECT = 222
KO_EFFECT_SPEED = 224
KO_EFFECT_INTENSITY = 226


def encode_dpt(dpt: str, value: object) -> bytes:
    if dpt == "DPT1":
        if not isinstance(value, bool):
            raise ValueError("DPT1 requires bool")
        return bytes([int(value)])
    if dpt == "DPT3":
        direction, step = value
        if direction not in (0, 1) or step not in range(8):
            raise ValueError("DPT3 direction/step out of range")
        return bytes([(direction << 3) | step])
    if dpt == "DPT5":
        if not isinstance(value, int) or not 0 <= value <= 255:
            raise ValueError("DPT5 value out of range")
        return bytes([value])
    if dpt == "DPT17":
        if not isinstance(value, int) or not 0 <= value <= 63:
            raise ValueError("DPT17 scene out of range")
        return bytes([value])
    if dpt == "DPT232":
        if not isinstance(value, int) or not 0 <= value <= 0xFFFFFF:
            raise ValueError("DPT232 RGB out of range")
        return value.to_bytes(3, "big")
    raise ValueError(f"unsupported DPT: {dpt}")


def decode_dpt(dpt: str, payload: bytes) -> object:
    if dpt == "DPT1" and len(payload) == 1:
        return bool(payload[0])
    if dpt == "DPT3" and len(payload) == 1:
        return (payload[0] >> 3 & 1, payload[0] & 7)
    if dpt in ("DPT5", "DPT17") and len(payload) == 1:
        return payload[0]
    if dpt == "DPT232" and len(payload) == 3:
        return int.from_bytes(payload, "big")
    raise ValueError(f"invalid {dpt} payload")


@dataclass
class SimulatedGroupObject:
    number: int
    raw_value: object
    dpt: str | None = None

    @classmethod
    def from_dpt(cls, number: int, dpt: str, value: object) -> "SimulatedGroupObject":
        return cls(number, encode_dpt(dpt, value), dpt)

    def asap(self) -> int:
        return self.number

    def value(self, _dpt: object) -> object:
        return decode_dpt(self.dpt, self.raw_value) if self.dpt else self.raw_value


@dataclass
class SimulatedController:
    nightlight_duration: int = 3
    nightlight_mode: int = 1
    nightlight_target: int = 0
    default_brightness: int = 255
    default_palette: int = 0
    default_effect: int = 0
    default_effect_speed: int = 128
    default_effect_intensity: int = 128
    brightness: int = 0
    red: int = 0
    green: int = 0
    blue: int = 0
    on: bool = False
    nightlight: bool = False
    preset: int = -1
    palette: int = 0
    effect: int = 0
    effect_speed: int = 0
    effect_intensity: int = 0
    commands: list[dict] = field(default_factory=list)
    feedback: dict[str, object] = field(default_factory=dict)
    feedback_writes: list[tuple[str, object]] = field(default_factory=list)

    def __post_init__(self) -> None:
        if not 1 <= self.nightlight_duration <= 255 or not 0 <= self.nightlight_mode <= 3:
            raise ValueError("invalid nightlight parameters")
        byte_parameters = (self.nightlight_target, self.default_brightness, self.default_palette, self.default_effect,
                           self.default_effect_speed, self.default_effect_intensity)
        if any(not 0 <= value <= 255 for value in byte_parameters):
            raise ValueError("invalid byte parameter")
        self.brightness = self.default_brightness
        self.palette = self.default_palette
        self.effect = self.default_effect
        self.effect_speed = self.default_effect_speed
        self.effect_intensity = self.default_effect_intensity
        self.on = self.brightness != 0
        self._publish_defaults()

    def _send(self, command: dict) -> None:
        self.commands.append(command)

    def _publish_defaults(self) -> None:
        self._set_feedback("brightness", self.brightness)
        self._set_feedback("on", self.on)
        self._set_feedback("palette", self.palette)
        self._set_feedback("effect", self.effect)
        self._set_feedback("effect_speed", self.effect_speed)
        self._set_feedback("effect_intensity", self.effect_intensity)

    def _set_feedback(self, key: str, value: object) -> None:
        if self.feedback.get(key) == value:
            return
        self.feedback[key] = value
        self.feedback_writes.append((key, value))

    def apply_wled_state(self, frame: str) -> None:
        root = json.loads(frame)
        state = root["state"]
        if "on" in state:
            self.on = bool(state["on"])
            self._set_feedback("on", self.on)
        if "bri" in state:
            self.brightness = int(state["bri"])
            self._set_feedback("brightness", self.brightness)
        if "nl" in state and "on" in state["nl"]:
            self.nightlight = bool(state["nl"]["on"])
            self._set_feedback("nightlight", self.nightlight)
        if "ps" in state:
            self.preset = int(state["ps"])
            self._set_feedback("preset", self.preset)
            if 1 <= self.preset <= 64:
                self._set_feedback("scene", self.preset - 1)
        segment = state.get("seg", [{}])[0]
        for key, attribute in (("pal", "palette"), ("fx", "effect"), ("sx", "effect_speed"), ("ix", "effect_intensity")):
            if key in segment:
                value = int(segment[key])
                setattr(self, attribute, value)
                self._set_feedback(attribute, value)

    def process_input(self, ko: SimulatedGroupObject) -> None:
        value = ko.value(None)
        if ko.asap() == KO_DIM_SW:
            self.on = bool(value)
            self._send({"on": self.on})
            self._set_feedback("on", self.on)
        elif ko.asap() == KO_DIM_ABS:
            self.brightness = max(0, min(255, int(value)))
            self.on = self.brightness != 0
            self._send({"bri": self.brightness})
            self._set_feedback("brightness", self.brightness)
            self._set_feedback("on", self.on)
        elif ko.asap() == KO_DIM_REL:
            direction, step = value
            if 1 <= step <= 7:
                delta = round(255 / (1 << (step - 1)))
                self.brightness = max(0, min(255, self.brightness + (delta if direction else -delta)))
                self.on = self.brightness != 0
                self._send({"bri": self.brightness})
                self._set_feedback("brightness", self.brightness)
                self._set_feedback("on", self.on)
        elif ko.asap() in (KO_RED, KO_GREEN, KO_BLUE, KO_RGB):
            if ko.asap() == KO_RGB:
                self.red, self.green, self.blue = (value >> 16) & 255, (value >> 8) & 255, value & 255
            elif ko.asap() == KO_RED:
                self.red = int(value)
            elif ko.asap() == KO_GREEN:
                self.green = int(value)
            else:
                self.blue = int(value)
            self.on = any((self.red, self.green, self.blue))
            self._send({"on": self.on, "seg": [{"col": [[self.red, self.green, self.blue]]}]})
            self._set_feedback("on", self.on)
            self._set_feedback("red", self.red)
            self._set_feedback("green", self.green)
            self._set_feedback("blue", self.blue)
            self._set_feedback("rgb", (self.red << 16) | (self.green << 8) | self.blue)
        elif ko.asap() == KO_NIGHTLIGHT:
            self.nightlight = bool(value)
            self._send({"nl": {"on": self.nightlight, "dur": self.nightlight_duration, "mode": self.nightlight_mode, "tbri": self.nightlight_target}})
            self._set_feedback("nightlight", self.nightlight)
        elif ko.asap() in (KO_SCENE, KO_PRESET):
            self.preset = int(value) + 1 if ko.asap() == KO_SCENE else int(value)
            self._send({"ps": self.preset})
            self._set_feedback("preset", self.preset)
            if 1 <= self.preset <= 64:
                self._set_feedback("scene", self.preset - 1)
        elif ko.asap() in (KO_PALETTE, KO_EFFECT, KO_EFFECT_SPEED, KO_EFFECT_INTENSITY):
            key = {KO_PALETTE: "pal", KO_EFFECT: "fx", KO_EFFECT_SPEED: "sx", KO_EFFECT_INTENSITY: "ix"}[ko.asap()]
            setattr(self, {"pal": "palette", "fx": "effect", "sx": "effect_speed", "ix": "effect_intensity"}[key], int(value))
            self.on = True
            self._send({"on": True, "seg": [{key: int(value)}]})
            self._publish_defaults()


@dataclass
class PollScheduler:
    interval_seconds: int
    first_request: bool = True
    last_request_ms: int = 0

    def due(self, now_ms: int) -> bool:
        if self.first_request:
            self.first_request = False
            self.last_request_ms = now_ms
            return True
        if self.interval_seconds == 0:
            return False
        if now_ms - self.last_request_ms >= self.interval_seconds * 1000:
            self.last_request_ms = now_ms
            return True
        return False
