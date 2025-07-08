import unittest
import os
from run_script import run_script, clean_output

test_name = os.path.basename(__file__)

class TestKeepsData(unittest.TestCase):
    def test_data_after_closing(self):
        result1 = run_script([
            "insert 1 user1 person1@example.com;",
            ".exit", 
        ], test_name,
            "test_data_after_closing")
        self.assertEqual(result1, ["sqlcs50> Executed.", "sqlcs50>"])

        result2 = run_script([
            "select;",
            ".exit;",
        ], test_name, "test_data_after_closing", overwrite=False)
        self.assertEqual(result2, ['sqlcs50> (1, user1, person1@example.com)', 'Executed.', 'sqlcs50>'])

