import requests
from pymongo import MongoClient
import time

# MongoDB connection
client = MongoClient('mongodb+srv://u:abc@cluster0.hc9hd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0')
db = client['IOT']
collection = db['data']

# ThingSpeak API endpoint
channel_id = "2637360"  # Remove leading space from Channel ID
api_key = "ICRJMFA4WFTKDZRG"  # Replace with your ThingSpeak API key
url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json?api_key={api_key}&results=2"  # Adjust the endpoint for the feeds



def get_current_ist_time():
    ist = pytz.timezone('Asia/Kolkata')
    current_time = datetime.now(ist)
    return current_time.strftime('%Y-%m-%d %H:%M:%S')



def fetch_data():
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        print("API Response:", data)  # Debugging: Print the API response
        feeds = data.get('feeds', [])
        print("Feeds:", feeds)  # Debugging: Print the feeds

        if feeds:
            latest_feed = feeds[0]  # Get the latest feed
            # Prepare the document for MongoDB with safe access
            document = {
                "Temprature": float(latest_feed.get('field1', 0)),  # Convert to float
                "Humidity": float(latest_feed.get('field2', 0)),        # Convert to float
                "Rain": int(latest_feed.get('field3', '0').strip()),                  # Convert to integer
                "CreatedAt": get_current_ist_time() # Add IST timestamp
            }
            return document
    else:
        print("Error fetching data from ThingSpeak:", response.status_code)
    return None

def store_data():
    while True:
        data = fetch_data()
        if data:
            collection.insert_one(data)  # Store data in MongoDB
            print("Data stored:", data)
        else:
            print("No data to store.")
        time.sleep(60)  # Wait for 1 minute before fetching again

if _name_ == '_main_':
    store_data()