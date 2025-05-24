import unittest
import os
from run_script import run_script, clean_output

test_name = os.path.basename(__file__)

class TestInsertSelect(unittest.TestCase):
    def test_insert_and_select(self):
        result = run_script([
            "insert 0 user0 person0@example.com;",
            "select;",
            ".exit;",
        ], test_name, "test_insert_and_select")
        self.assertEqual(result, [
            "sqlcs50> Executed.",
            "sqlcs50> (0, user0, person0@example.com)",
            "Executed.",
            "sqlcs50>",
        ])
    