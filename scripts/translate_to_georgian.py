import re

MAPPING = [
    ("adze", "აძე"),
    ("aze", "აძე"),
    ("ch", "ჩ"),
    ("ch", "ჩ"),
    ("chj", "ჭ"),
    ("dz", "ძ"),
    ("dz", "ძ"),
    ("edze", "ეძე"),
    ("eze", "ეძე"),
    ("gh", "ღ"),
    ("gh", "ღ"),
    ("ia", "ია"),
    ("idze", "იძე"),
    ("ize", "იძე"),
    ("kh", "ხ"),
    ("kh", "ხ"),
    ("kk", "კ"),
    ("ps", "ფ"),
    ("ps", "ფ"),
    ("sh", "შ"),
    ("sh", "შ"),
    ("svili", "შვილი"),
    ("sz", "შ"),
    ("tch", "ჭ"),
    ("th", "თ"),
    ("th", "თ"),
    ("tj", "ჭ"),
    ("tj", "ჭ"),
    ("ts", "ც"),
    ("ts", "ც"),
    ("tsh", "წ"),
    ("tt", "ტ"),
    ("ua", "უა"),
    ("zh", "ჟ"),
    ("a", "ა"),
    ("b", "ბ"),
    ("c", "ც"),
    ("d", "დ"),
    ("e", "ე"),
    ("f", "ფ"),
    ("g", "გ"),
    ("h", "ჰ"),
    ("i", "ი"),
    ("j", "ჯ"),
    ("k", "კ"),
    ("l", "ლ"),
    ("m", "მ"),
    ("n", "ნ"),
    ("o", "ო"),
    ("p", "პ"),
    ("q", "ქ"),
    ("r", "რ"),
    ("s", "ს"),
    ("t", "თ"),
    ("u", "უ"),
    ("v", "ვ"),
    ("w", "წ"),
    ("x", "ხ"),
    ("y", "ყ"),
    ("z", "ზ"),
]


KEY_MAP = {
    "saxeli": "სახელი",
    "gvari": "გვარი",
    "piadi": "პირადი ნომერი",
    "piadi #": "პირადი ნომერი",
    "sqesi": "სქესი",
    "dab weli": "დაბადების თარიღი",
    "reg TariRi": "რეგისტრაციის თარიღი",
    "mocmobis #": "მოწმობის ნომერი",
    "quCa": "ქუჩა",
    "REGMDAT": "რეგისტრაციის ადგილი",
    "mamis saxeli": "მამის სახელი",
    "mamisSaxeli": "მამის სახელი",
    "mamisPiradi": "მამის პირადი",
    "dedisSaxeli": "დედის სახელი",
    "dedisPiradi": "დედის პირადი",
    "raioni": "რაიონი",
}


def georgianize_value(text):
    if not text or text.isdigit():
        return text

    res = text.lower()
    for eng, geo in MAPPING:
        res = res.replace(eng, geo)
    return res


def process_huge_file(input_path, output_path):
    # Regex for "key": "value"
    pattern = re.compile(r'"([^"]+)"\s*:\s*"([^"]+)"')

    with (
        open(input_path, encoding="utf-8") as f_in,
        open(output_path, "w", encoding="utf-8") as f_out,
    ):
        for line in f_in:

            def subst(m):
                k, v = m.group(1), m.group(2)
                # Map key if known, else keep original
                new_key = KEY_MAP.get(k, k)
                # Transliterate value
                new_val = georgianize_value(v)
                return f'"{new_key}": "{new_val}"'

            f_out.write(pattern.sub(subst, line))


if __name__ == "__main__":
    process_huge_file("merged_database.json", "data_georgian.json")
