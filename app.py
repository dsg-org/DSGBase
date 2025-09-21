import os
from flask import Flask
from werkzeug.security import generate_password_hash
from pysrc.ext import db, login_manager
from pysrc.models import User
from pysrc.routes import register_routes
from pysrc.auth_routes import auth
from pysrc.profile_routes import profile_bp
import platform # ააააააააააააააა


BASE_DIR = os.path.abspath(os.path.dirname(__file__))
DB_PATH = os.path.join(BASE_DIR, 'instance', 'database.db')

app = Flask(__name__)
app.config["SECRET_KEY"] = "kascbaSBAJBJ!@#3546"
app.config["SQLALCHEMY_DATABASE_URI"] = f"sqlite:///{DB_PATH}"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False

os.makedirs(os.path.dirname(DB_PATH), exist_ok=True)

db.init_app(app)
login_manager.init_app(app)
login_manager.login_view = "auth.login"

@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))

app.register_blueprint(auth)
app.register_blueprint(profile_bp)
register_routes(app)

with app.app_context():
    try:
        db.create_all()
        if not User.query.filter_by(username="admin").first():
            admin = User(
                username="admin",
                email="admin@admin",
                password=generate_password_hash("adminadmin321"),
                role="admin"
            )
            db.session.add(admin)
            db.session.commit()
            print("admini aqainti sheiqmna ")
    except Exception as e:
        print(f"Error: {str(e)}")
        if 'db' in locals() and db.session:
            db.session.rollback()

if __name__ == "__main__":
    # if platform.system() == "Windows":
    #     os.system(f'netsh advfirewall firewall add rule name="Python Flask App" dir=in action=allow protocol=TCP localport=5000')
    
     app.run(host="0.0.0.0", port=5000, debug=True)