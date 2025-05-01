import unittest
from run_script import run_script, clean_output

class TestKeepsData(unittest.TestCase):
    def test_keeps_data_after_closing_connection(self):
        result1 = run_script([
            "insert 1 user1 person1@example.com;",
            ".exit;",
        ])
        cleaned1 = clean_output(result1)
        self.assertEqual(cleaned1, ["Executed.", ""])

        result2 = run_script([
            "select;",
            ".exit;",
        ])
        cleaned2 = clean_output(result2)
        self.assertEqual(cleaned2, [
            "(1, user1, person1@example.com)",
            "Executed.",
            "",
        ])
