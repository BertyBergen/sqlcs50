import subprocess

def run_script(commands):
    input_script = "\n".join(commands) + "\n"
    try:
        process = subprocess.Popen(
            ["sqlcs50.exe", "test.db"],
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
