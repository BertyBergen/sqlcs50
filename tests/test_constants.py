import unittest
from run_script_helper import run_script, clean_output

class TestConstants(unittest.TestCase):
    def test_prints_constants(self):
        script = [
            ".constants;",
            ".exit;",
        ]
        result = run_script(script)
        cleaned = clean_output(result)
        self.assertIn("Constants:", "\n".join(cleaned))
        self.assertIn("ROW_SIZE:", "\n".join(cleaned))
