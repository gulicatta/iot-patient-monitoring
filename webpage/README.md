# Webpage

React frontend + Flask backend.

## Docker (recommended)

Builds the frontend and bundles everything into a single container.
Flask serves both the API and the static files.

```bash
docker compose up --build
```

App runs on http://localhost:5000.

---

## Local development

Backend and frontend run separately. Vite proxies `/api` to Flask,
so both servers must be running at the same time.

### Backend

```bash
cd backend
python -m venv .venv
source .venv/Scripts/activate   # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python api.py
```

### Frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend runs on http://localhost:5173, backend on http://localhost:5000.

---

## UART Connector (optional)

Reads sensor packets from a connected Launchpad and POSTs them to the backend.
Run in a separate terminal with the venv activated:

```bash
python uart_connector.py --port COM3        # real hardware
python uart_connector.py --mock             # simulated data
```
