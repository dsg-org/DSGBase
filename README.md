# DSGBase

DSGBase is a hybrid **C backend + Flask frontend** framework for working with user datasets.
It provides efficient backend logic for data storage/search and a web frontend for user interaction.

---

## Overview

* **Backend (C)**

  * Converts user records from JSON into a compact binary format.
  * Supports filtered searches by name, surname, ID, or district.
  * Provides utilities for loading, parsing, and printing user data.

* **Frontend (Flask)**

  * Simple web interface for interacting with the backend.
  * Uses `Flask`, `Flask-SQLAlchemy`, `Flask-Login`, and `Flask-WTF`.
  * Templates and static files are stored in `templates/` and `static/`.

* **Dev Environment (Nix/Flakes)**

  * Reproducible setup including C libraries and Python tooling.
  * Ready-to-use development shell with formatters, compilers, and libraries.

---

## Features

* JSON → Binary conversion
* Filtered search by multiple fields (`-n`, `-s`, `-i`, `-r`)
* Print user surnames (`-p`)
* Flask-based web interface with templates/static assets
* Nix-based development environment (Linux)
* Makefile for simplified builds

---

## Dependencies

If using the provided `flake.nix`, the following dependencies are included automatically:

### Python (Frontend)

* `flask`
* `flask-sqlalchemy`
* `flask-login`
* `flask-wtf`
* `pip`

### C / Development (Backend)

* `uthash` — hash table utilities
* `cjson` — C JSON parser
* `clang-tools` — linting, static analysis
* `bear` — build tooling for C/C++
* `black` — Python code formatter
* `alejandra` — Nix code formatter

---

## Build & Run

### Backend (C logic)

Using `make`:

```bash
make
```

Manually:

```bash
gcc -o dsgbase main.c file_handling.c user_handling.c
```

Run:

```bash
./dsgbase [OPTIONS]
```

### Frontend (Flask app)

Activate the dev shell:

```bash
nix develop
```

Then run:

```bash
python app.py
```

Access the app at [http://localhost:5000](http://localhost:5000).

---

## Command-Line Options

```
  -b <json_file>    Convert JSON file to binary
  -f <binary_file>  Specify binary file to search
  -n <name>         Search by name
  -s <surname>      Search by surname
  -i <id>           Search by ID
  -r <district>     Search by district
  -p                Print surnames
```

---

## Examples

Convert a JSON file:

```bash
./dsgbase -b users.json
```

Search users by district:

```bash
./dsgbase -f users.bin -r Central
```

Run frontend server:

```bash
nix develop
python app.py
```

---

## Project Structure

```
.
├── main.c              # backend entrypoint
├── file_handling.c/.h  # JSON/Binary I/O
├── user_handling.c/.h  # User structs & search
├── app.py              # Flask frontend
├── templates/          # HTML templates
├── static/             # CSS, JS, images
├── flake.nix           # Nix dev environment
├── requirements.txt    # Python deps (alt to flake)
├── Makefile
└── ...
```
