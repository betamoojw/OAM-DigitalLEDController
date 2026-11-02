import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).parents[1]
NS = {"knx": "http://knx.org/xml/project/20", "op": "http://github.com/OpenKNX/OpenKNXproducer"}


class EtsXmlTests(unittest.TestCase):
    def test_development_and_release_products_have_compatibility_metadata(self):
        for filename in ("DigitalLEDController.xml", "DigitalLEDController-Release.xml"):
            root = ET.parse(ROOT / "src" / filename).getroot()
            self.assertEqual(root.tag, "{http://knx.org/xml/project/20}KNX")
            minimum = root.find("op:minOpenKNXproducerVersion", NS)
            self.assertIsNotNone(minimum)
            self.assertEqual(minimum.attrib["value"], "4.3.12")
            self.assertIsNotNone(root.find("op:ETS", NS))
            self.assertIsNotNone(root.find("knx:generate", NS))

    def test_development_and_release_products_include_config_transfer(self):
        config_root = ET.parse(ROOT / "src" / "DigitalLEDController.conf.xml").getroot()
        config_values = {node.attrib["name"]: node.attrib["value"] for node in config_root.findall("op:config", NS)}
        self.assertEqual(config_values["%UCT_VerifyVersion%"], "0.5")

        for filename in ("DigitalLEDController.xml", "DigitalLEDController-Release.xml"):
            root = ET.parse(ROOT / "src" / filename).getroot()
            modules = {node.attrib["prefix"]: node for node in root.findall("op:define", NS)}
            config_transfer = modules["UCT"]
            self.assertEqual(config_transfer.attrib["ModuleType"], "99")
            self.assertEqual(config_transfer.attrib["noConfigTransfer"], "true")
            self.assertEqual(config_transfer.attrib["share"], "../lib/OFM-ConfigTransfer/src/ConfigTransfer.share.xml")

            verify = config_transfer.find("op:verify", NS)
            self.assertIsNotNone(verify)
            self.assertEqual(verify.attrib["File"], "../lib/OFM-ConfigTransfer/library.json")
            self.assertEqual(verify.attrib["ModuleVersion"], "%UCT_VerifyVersion%")

    def test_module_xml_exposes_runtime_parameters_and_help(self):
        root = ET.parse(ROOT / "lib" / "OFM-DigitalLEDController" / "src" / "DigitalLedModule.share.xml").getroot()
        names = {node.attrib["Name"] for node in root.findall(".//knx:Parameter", NS)}
        self.assertTrue({
            "RequestXInterval",
            "NightlightDuration",
            "NightlightMode",
            "NightlightTargetBrightness",
            "DefaultBrightness",
            "DefaultPalette",
            "DefaultEffect",
            "DefaultEffectSpeed",
            "DefaultEffectIntensity",
        }.issubset(names))

        template = ET.parse(ROOT / "lib" / "OFM-DigitalLEDController" / "src" / "DigitalLedModule.templ.xml").getroot()
        visible_help = " ".join(node.attrib.get("Text", "") for node in template.iter())
        self.assertIn("Statusabfrage", visible_help)
        self.assertIn("Nachtlicht", visible_help)
        self.assertIn("Startwerte und Effekte", visible_help)


if __name__ == "__main__":
    unittest.main()
