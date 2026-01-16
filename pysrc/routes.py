from datetime import datetime
import json
import os
import subprocess
import threading

from flask import flash, jsonify, redirect, render_template, request, url_for
from flask_login import current_user, login_required

from folderRoad import folderRoad
from pysrc.ext import db
from pysrc.forms import LoginForm, RegisterForm
from pysrc.models import User

process_lock = threading.Lock()


def register_routes(app):
    @app.route("/profile/update", methods=["POST"])
    @login_required
    def update_profile():
        nickname = request.form.get("nickname", "").strip()
        district = request.form.get("district", "").strip()

        current_user.nickname = nickname
        current_user.district = district

        try:
            db.session.commit()
            flash("Profile updated successfully!", "success")
        except Exception:
            db.session.rollback()
            flash("An error occurred while updating profile.", "danger")

        return redirect(url_for("profile"))

    @app.route("/")
    def index():
        login_form = LoginForm()
        register_form = RegisterForm()

        return render_template(
            "index.html",
            login_form=login_form,
            register_form=register_form,
            show_login_overlay=not current_user.is_authenticated,
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
        if not current_user.is_admin():
            flash("You don't have permission to view this page.", "danger")
            return redirect(url_for("index"))

        users = User.query.all()
        return render_template("PageUsers/PageUsers.html", users=users)

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

        if not current_user.is_admin():
            now = datetime.utcnow()
            if (
                current_user.search_last_reset is None
                or current_user.search_last_reset.date() < now.date()
            ):
                current_user.search_count = 0
                current_user.search_last_reset = now
                db.session.commit()

            if current_user.search_count >= 5:
                return {"error": "Daily limit exceeded."}, 429

            current_user.search_count += 1
            db.session.commit()

        if not process_lock.acquire(blocking=False):
            return {"error": "Search already running"}, 429

        try:
            data = request.json or {}
            binary_path = os.path.join(folderRoad(), "main")
            data_path = os.path.join(folderRoad(), "src", "output.bin")

            args = [binary_path, "-f", data_path]

            mapping = {
                "name": "-n",
                "surname": "-s",
                "id": "-i",
                "district": "-r",
            }

            for key, flag in mapping.items():
                val = data.get(key)
                if val:
                    args += [flag, str(val).strip()]
                    print(args)

            result = subprocess.run(
                args, capture_output=True, text=True, timeout=300
            )
            print(result)

            if result.returncode != 0:
                return {"error": f"C Backend Error: {result.stderr}"}, 500

            raw_output = result.stdout.strip()

            json_start = raw_output.find("[")
            json_end = raw_output.rfind("]") + 1

            if json_start == -1:
                return jsonify([])

            json_part = raw_output[json_start:json_end]
            return jsonify(json.loads(json_part))

        except Exception as e:
            return {"error": str(e)}, 500
        finally:
            process_lock.release()

    @app.route("/api/summary", methods=["GET"])
    @login_required
    def get_summary():
        try:
            with open("static/data/Desc.json", encoding="utf-8") as f:
                return jsonify(json.load(f))
        except Exception as e:
            return {"error": str(e)}, 500
