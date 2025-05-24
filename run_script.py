import subprocess
import os

def run_script(commands, test_name, unit_test_name="default_test", overwrite=True):
    db_dir = os.path.join("./tests/tests_output", test_name)
    os.makedirs(db_dir, exist_ok=True)
    
    db_filename = os.path.join(db_dir, f"{unit_test_name}.db")

    # Удаляем старый файл, если есть\
    if overwrite and os.path.exists(db_filename):
        os.remove(db_filename)

    input_script = "\n".join(commands) + "\n"

    try:
        process = subprocess.Popen(
            ["sqlcs50.exe", db_filename],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        raw_output, _ = process.communicate(input=input_script, timeout=10)
        return [line.strip() for line in raw_output.strip().split("\n")]
    
    except subprocess.TimeoutExpired:
        process.kill()
        raise Exception("The program took too long to respond.")


def clean_output(result):
    return [line.replace("sqlcs50> ", "") for line in result]
