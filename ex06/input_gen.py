import string
import random

FILE_NAME = "input.txt"

def generate_string(length):
    """Generate a string of a given length with printable characters (excluding whitespace)"""
    chars = string.ascii_letters + string.digits + string.punctuation
    return ''.join(random.choice(chars) for _ in range(length))

def main():
    n = int(input("Enter the length of the string (N): "))

    with open(FILE_NAME, 'w') as f:
        f.write(generate_string(n))

    print(f"String of length {n} saved to {FILE_NAME}.")

if __name__ == "__main__":
    main()
