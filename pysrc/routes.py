from flask import render_template, request, jsonify, redirect, url_for
from flask_login import current_user, login_required
import os
import subprocess
import json
import threading
from flask import flash
from pysrc.ext import db
from failisGza import failisGza
from pysrc.models import User

process_lock = threading.Lock()


def register_routes(app):
    @app.route("/profile/update", methods=["POST"])
    @login_required
    def update_profile():
        nickname = request.form.get("nickname", "").strip()
        region = request.form.get("region", "").strip()

        current_user.nickname = nickname
        current_user.region = region

        try:
            db.session.commit()
            flash("Profile updated successfully!", "success")
        except Exception:
            db.session.rollback()
            flash("An error occurred while updating profile.", "danger")

        return redirect(url_for("profile"))

    @app.route("/")
    def index():
        return render_template(
            "index.html", show_login_overlay=not current_user.is_authenticated
        )

    @app.route("/about")
    @login_required
    def about():
        return render_template("About/about.html")

    @app.route("/profile")
    @login_required
    def profile():
        return render_template("Profile/profile.html", user=current_user)

    @app.route("/description")
    @login_required
    def description():
        return render_template("Description/description.html")
    @app.route("/PageUsers")
    @login_required
    def PageUsers():
        users = User.query.all()
        print(users)
        if not current_user.is_admin():
            return redirect(url_for("index"))
        print("ddd")
        return render_template("PageUsers/PageUsers.html" ,users=users)
    @app.route("/PageUser/<int:user_id>")
    @login_required
    def PageUser(user_id):  
        if not current_user.is_admin():
             return redirect(url_for("index"))
        user = User.query.get_or_404(user_id)
        return render_template("PageUsers/PageUser.html", user=user) 

    @app.route("/api/search", methods=["POST"])
    @login_required
    def api_search():
        token = request.headers.get("Authorization")
        if token != "Bearer 12345":
            return {"error": "Unauthorized"}, 401
    
        if not process_lock.acquire(blocking=False):
            return {"error": "Search already running"}, 429
    
        data = request.json or {}
        binary_path = os.path.join(failisGza(), "main")
        data_path = os.path.join(failisGza(), "src", "output.bin")


    
        args = [binary_path, "-f", data_path]
        if "name" in data:
            args += ["-n", data["name"].lower()]
        if "surname" in data:
            args += ["-s", data["surname"].lower()]
        if "id" in data:
            args += ["-i", data["id"]]
        if "region" in data:
            args += ["-r", data["region"].lower()]
    
        print("Executing command:", " ".join(args))
    
        try:
            result = subprocess.run(args, capture_output=True, text=True, timeout=300)
            raw_output = result.stdout.strip()
    
            print("Command output:", raw_output)
    
            json_start = raw_output.find("[")
            json_end = raw_output.rfind("]") + 1
            json_part = raw_output[json_start:json_end].strip()
    
            if not json_part.startswith("[") or not json_part.endswith("]"):
                return {"error": "Invalid format"}, 500
    
            parsed = json.loads(json_part)
    
            return jsonify(parsed)
    
        except subprocess.TimeoutExpired:
            print("Error: Command timed out.")
            return {"error": "Timeout"}, 504
    
        except Exception as e:
            print("Error:", str(e))
            return {"error": str(e)}, 500
    
        finally:
            process_lock.release()
    @app.route("/api/summary", methods=["GET"])
    @login_required
    def get_summary():
        try:
            with open("static/data/Desc.json", "r", encoding="utf-8") as f:
                return jsonify(json.load(f))
        except Exception as e:
            return {"error": str(e)}, 500
        
