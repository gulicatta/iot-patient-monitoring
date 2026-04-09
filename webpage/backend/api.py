import json
import os

from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS

import patient_store

_DIST = os.path.join(os.path.dirname(__file__), "dist")

# Load config
_CONFIG_PATH = os.path.join(os.path.dirname(__file__), "config.json")
with open(_CONFIG_PATH) as _f:
    _CONFIG = json.load(_f)

_PILL_INTERVAL = int(_CONFIG["pill_timer"]["interval_seconds"])

app = Flask(__name__, static_folder=None)
CORS(app)


@app.route("/api/health", methods=["GET"])
def health():
    return jsonify({"status": "ok"})


@app.route("/api/config", methods=["GET"])
def get_config():
    """Serve frontend-relevant configuration (thresholds, etc.)."""
    return jsonify(_CONFIG)


@app.route("/api/ingest", methods=["POST"])
def ingest():
    """Receive a JSON sensor packet from a uart_connector instance."""
    body = request.get_json(silent=True)
    if not body or "id" not in body:
        return jsonify({"error": "JSON body with 'id' field required"}), 400
    patient_store.ingest(body)
    if body.get("b2"):
        patient_store.confirm_pill(body.get("id"), _PILL_INTERVAL)
    return jsonify({"ok": True}), 200


@app.route("/api/data", methods=["GET"])
def get_data():
    data = [
        {"id": 1, "sensor": "temperature", "value": 22.5, "unit": "°C"},
        {"id": 2, "sensor": "humidity", "value": 48.0, "unit": "%"},
        {"id": 3, "sensor": "pressure", "value": 1013.25, "unit": "hPa"},
    ]
    return jsonify(data)


@app.route("/api/data", methods=["POST"])
def post_data():
    body = request.get_json()
    if not body:
        return jsonify({"error": "No JSON body provided"}), 400
    return jsonify({"received": body}), 201


@app.route("/api/patients", methods=["GET"])
def get_patients():
    """Return all known patients with their latest reading."""
    return jsonify(patient_store.get_all_patients())


@app.route("/api/patients/<did>", methods=["GET"])
def get_patient(did):
    """Return the latest reading for a single patient."""
    result = patient_store.get_patient(did)
    if result is None:
        return jsonify({"error": f"Patient {did} not found"}), 404
    return jsonify(result)


@app.route("/api/patients/<did>/history", methods=["GET"])
def get_patient_history(did):
    """Return the last N readings for a patient (oldest first). ?n=50 max."""
    try:
        n = min(int(request.args.get("n", 50)), 50)
    except (ValueError, TypeError):
        n = 50

    result = patient_store.get_patient_history(did, n)
    if result is None:
        return jsonify({"error": f"Patient {did} not found"}), 404
    return jsonify(result)


@app.route("/api/patients/<did>/pill", methods=["GET"])
def get_pill_status(did):
    """Return pill timer status for a patient."""
    result = patient_store.get_pill_status(did, _PILL_INTERVAL)
    if result is None:
        return jsonify({"error": f"Patient {did} not found"}), 404
    return jsonify(result)


@app.route("/api/patients/<did>/pill/confirm", methods=["POST"])
def confirm_pill(did):
    """Record that a pill was taken now."""
    result = patient_store.confirm_pill(did, _PILL_INTERVAL)
    return jsonify(result), 200


@app.route("/", defaults={"path": ""})
@app.route("/<path:path>")
def serve_frontend(path):
    full = os.path.join(_DIST, path)
    if path and os.path.exists(full):
        return send_from_directory(_DIST, path)
    return send_from_directory(_DIST, "index.html")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
