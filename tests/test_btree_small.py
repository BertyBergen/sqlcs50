import os
import unittest
from run_script import run_script, clean_output

test_name = os.path.basename(__file__)
class TestBtreeSmall(unittest.TestCase):
    def test_one_node_btree(self):
        script = [
            "insert 3 user3 person3@example.com;",
            "insert 1 user1 person1@example.com;",
            "insert 2 user2 person2@example.com;",
            ".btree;",
            ".exit;",
        ]
        result = run_script(script, test_name, "test_one_node_btree")
        expected_output = [
            "sqlcs50> Executed.",
            "sqlcs50> Executed.",
            "sqlcs50> Executed.",
            "sqlcs50> Tree:",
            "- leaf (size 3)",
            "- 1",
            "- 2",
            "- 3",
            "sqlcs50>"]
        self.assertEquals(result, expected_output)
    def test_three_leaf_node_btree(self):
        script = [f"insert {i} user{i} person{i}@example.com;" for i in range(1, 15)]
        script += [
            ".btree;",
            "insert 15 user15 person15@example.com;",
            ".exit;",
        ]
        result = run_script(script, test_name, "test_three_leaf_node_btree")
        expected_output = ["sqlcs50> Executed."] *14
        expected_output += [
            "sqlcs50> Tree:",
            "- internal (size 1)",
            "- leaf (size 7)",
            "- 1",
            "- 2",
            "- 3",
            "- 4",
            "- 5",
            "- 6",
            "- 7",
            "- key 7",
            "- leaf (size 7)",
            "- 8",
            "- 9",
            "- 10",
            "- 11",
            "- 12",
            "- 13",
            "- 14",
            "sqlcs50> Executed.",
            "sqlcs50>"
        ]
        self.assertEqual(result, expected_output)

