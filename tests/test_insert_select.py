import unittest
from run_script_helper import run_script, clean_output

class TestInsertSelect(unittest.TestCase):
    def test_insert_and_select(self):
        result = run_script([
            "insert 0 user0 person0@example.com;",
            "select;",
            ".exit;",
        ])
        self.assertEqual(result, [
            "sqlcs50> Executed.",
            "sqlcs50> (0, user0, person0@example.com)",
            "Executed.",
            "sqlcs50>",
        ])
    