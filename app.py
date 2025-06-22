from flask import Flask, jsonify, request, render_template
import subprocess
import json
import os
import threading
from collections import Counter

app = Flask(__name__)

process_lock = threading.Lock()


@app.route("/")
def base():
    return render_template("base.html")


@app.route("/about")
def about():
    return render_template("About/about.html")




@app.route("/settings")
def settings():
    return render_template("Settings/settings.html")


@app.route("/api/search", methods=["POST"])
def api_search():
    token = request.headers.get("Authorization")
    if token != "Bearer 12345":
        return {"error": "Unauthorized: Invalid or missing token."}, 401

    if not process_lock.acquire(blocking=False):
        return {"error": "Search is already in progress. Please wait..."}, 429

    data = request.json or {}

    base_path = os.path.dirname(os.path.abspath(__file__))
    binary_path = os.path.join(base_path, "main")
    data_path = os.path.join(base_path, "src", "output.bin")

    args = [binary_path, "-f", data_path]
    if "name" in data:
        args += ["-n", data["name"].lower()]
    if "surname" in data:
        args += ["-s", data["surname"].lower()]
    if "id" in data:
        args += ["-i", data["id"]]
    if "region" in data:
        args += ["-r", data["region"].lower()]

    try:
        result = subprocess.run(args, capture_output=True, text=True, timeout=300)
        raw_output = result.stdout.strip()

        json_start = raw_output.find("[")
        json_end = raw_output.rfind("]") + 1
        json_part = raw_output[json_start:json_end].strip()

        if not json_part.startswith("[") or not json_part.endswith("]"):
            return {"error": "Invalid response format from the search process."}, 500

        parsed = json.loads(json_part)
        return jsonify(parsed)

    except subprocess.TimeoutExpired:
        return {"error": "Search timed out after 5 minutes."}, 504

    except Exception as e:
        return {"error": f"Internal server error: {str(e)}"}, 500

    finally:
        process_lock.release()

@app.route("/api/summary", methods=["GET"])
def get_summary():
    json_path = os.path.join("data", "Desc.json")
    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)
    return jsonify(data)

@app.route("/description")
def description():
    return render_template("Description/description.html")

if __name__ == "__main__":
    app.run(debug=True)



