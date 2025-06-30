# create_db.py
from ext import db
from app import app
from models import User 

with app.app_context():
    db.create_all()
    print(" ბაზა და ცხრილები შეიქმნა.")
