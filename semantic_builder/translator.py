path_to_table_txt="/home/ubuntu/projects/lkm_samples/semantic_builder/table.txt"

def load_table():
    table = {}
    with open(path_to_table_txt, "r") as f:
        records = f.read().splitlines()
        for record in records:
            name, va, pa = record.split(" ")
            table[name] = (int(va, 16), int(pa, 16))
    return table

if __name__ == "__main__":
    table = load_table()
    while True:
        name = input("Enter a name: ")
        offset = input("Enter an offset: ")
        va, pa = table[name]
        print("Virtual address: %#x" % va)
        print("Physical address: %#x" % pa)
