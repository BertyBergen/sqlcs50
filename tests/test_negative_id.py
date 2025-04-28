import unittest
from run_script_helper import run_script

class TestNegativeID(unittest.TestCase):
    def test_prints_error_message_if_id_is_negative(self):
        script = [
            "insert -1 cstack foo@bar.com;",
            "select;",
            ".exit;",
        ]
        result = run_script(script)
        self.assertIn("sqlcs50 > ID must be positive.", result)
