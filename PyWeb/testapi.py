from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)

# Store received data in memory
received_data = []

# HTML template for the web page
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Received Meter Data</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 40px; background-color: #1e1e1e; color: #d4d4d4;}
        .container { max-width: 1000px; margin: auto; background: #2d2d2d; padding: 30px; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.5); }
        h1 { color: #569cd6; text-align: center; margin-bottom: 30px; }
        .data-item { 
            background-color: #1e1e1e; 
            border-left: 4px solid #ce9178; 
            padding: 15px; 
            margin-bottom: 10px; 
            border-radius: 3px; 
            font-family: Consolas, 'Courier New', monospace; 
            font-size: 16px; 
            white-space: pre-wrap; 
            word-wrap: break-word;
            color: #dcdcaa;
        }
        .no-data { color: #888; font-style: italic; text-align: center; margin-top: 50px; font-size: 1.2em; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Received Data Log</h1>
        <div id="data-container">
            <p class="no-data">Waiting for data...</p>
        </div>
    </div>

    <script>
        async function fetchData() {
            try {
                const response = await fetch('/api/get_data');
                const data = await response.json();
                
                const container = document.getElementById('data-container');
                
                if (data.length === 0) {
                    container.innerHTML = '<p class="no-data">No data received yet. Send a POST request to <b>/api/data</b></p>';
                    return;
                }
                
                container.innerHTML = '';
                // Show latest data first
                data.slice().reverse().forEach(item => {
                    const div = document.createElement('div');
                    div.className = 'data-item';
                    
                    // Reorder keys explicitly
                    const orderedItem = {};
                    const order = ["SN", "Time", "LValue", "AValue", "BValue", "LValue1", "AValue1", "BValue1", "LValue2", "AValue2", "BValue2"];
                    
                    // Put the requested keys first
                    order.forEach(key => {
                        if (item.hasOwnProperty(key)) {
                            orderedItem[key] = item[key];
                        }
                    });
                    
                    // Add any unexpected/other keys at the end just in case
                    Object.keys(item).forEach(key => {
                        if (!orderedItem.hasOwnProperty(key)) {
                            orderedItem[key] = item[key];
                        }
                    });
                    
                    // Replace double quotes with single quotes to mimic Python dictionary style exactly as requested
                    let dictString = JSON.stringify(orderedItem).replace(/"/g, "'");
                    // Add some spaces after commas for readability
                    dictString = dictString.replace(/,/g, ', ');
                    div.textContent = dictString;
                    
                    container.appendChild(div);
                });
            } catch (err) {
                console.error('Error fetching data:', err);
            }
        }

        // Fetch immediately and then poll every 500ms
        fetchData();
        setInterval(fetchData, 500);
    </script>
</body>
</html>
"""

@app.route('/', methods=['GET'])
def index():
    """Route to display the web page."""
    return render_template_string(HTML_TEMPLATE)

@app.route('/api/get_data', methods=['GET'])
def get_data():
    """API to return all received data to the web page via background fetching."""
    return jsonify(received_data)

@app.route('/api/data', methods=['POST'])
def receive_data():
    """Route to receive POST data from RestApi.cs."""
    if request.is_json:
        data = request.get_json()
        print(f"Received data: {data}")
        
        # Append the new data to our list
        received_data.append(data)
        
        return jsonify({"status": "success", "message": "Data received successfully"}), 200
    else:
        return jsonify({"status": "error", "message": "Request must be JSON"}), 400

if __name__ == '__main__':
    # Run the Flask app on port 5000
    print("Starting Flask server...")
    print("Web UI: http://localhost:5000/")
    print("API Endpoint for RestApi.cs: http://localhost:5000/api/data")
    app.run(host='0.0.0.0', port=5000, debug=True)
