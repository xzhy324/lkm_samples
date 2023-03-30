path_to_system_map = "/home/ubuntu/projects/lkm_samples/semantic_builder/System.map-5.15.0-67-generic"
path_to_table_txt = "/home/ubuntu/projects/lkm_samples/semantic_builder/table.txt"

cord = {
    "name": "sys_call_table",
    "va": "0xffffffffac000300",
    "pa": "0x00000000a2ae9092"
}
offset = 0

table = {}

fin = open(path_to_system_map, "r")
fout = open(path_to_table_txt, "w")

records = fin.read().splitlines()
for record in records:
    va, type_, name = record.split(" ")
    if int(va, 16) >= 0xffffffff00000000:
        table[name]= [int(va, 16), 0]

va_offset = int(cord["va"], 16) - table[cord["name"]][0]
va2pa_offset = int(cord["pa"], 16) - int(cord["va"], 16)



for name in table.keys():
    table[name][0] = table[name][0] + va_offset
    table[name][1] = table[name][0] + va2pa_offset
    msg ="%s %#X %#X\n" % (name, table[name][0], table[name][1])
    fout.write(msg)
    
fin.close()
fout.close()
    