const API_BASE = "/api";

export async function fetchHealth() {
  const res = await fetch(`${API_BASE}/health`);
  if (!res.ok) throw new Error("Health check failed");
  return res.json();
}

export async function fetchConfig() {
  const res = await fetch(`${API_BASE}/config`);
  if (!res.ok) throw new Error("Failed to fetch config");
  return res.json();
}

export async function fetchSensorData() {
  const res = await fetch(`${API_BASE}/data`);
  if (!res.ok) throw new Error("Failed to fetch sensor data");
  return res.json();
}

export async function postSensorData(payload) {
  const res = await fetch(`${API_BASE}/data`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
  if (!res.ok) throw new Error("Failed to post sensor data");
  return res.json();
}

// --- Patient monitoring ---

export async function fetchPatients() {
  const res = await fetch(`${API_BASE}/patients`);
  if (!res.ok) throw new Error("Failed to fetch patients");
  return res.json();
}

export async function fetchPatient(did) {
  const res = await fetch(`${API_BASE}/patients/${did}`);
  if (!res.ok) throw new Error(`Failed to fetch patient ${did}`);
  return res.json();
}

export async function fetchPatientHistory(did, n = 50) {
  const res = await fetch(`${API_BASE}/patients/${did}/history?n=${n}`);
  if (!res.ok) throw new Error(`Failed to fetch history for patient ${did}`);
  return res.json();
}

// --- Pill timer ---

export async function fetchPillStatus(did) {
  const res = await fetch(`${API_BASE}/patients/${did}/pill`);
  if (!res.ok) throw new Error(`Failed to fetch pill status for patient ${did}`);
  return res.json();
}

export async function confirmPill(did) {
  const res = await fetch(`${API_BASE}/patients/${did}/pill/confirm`, {
    method: "POST",
  });
  if (!res.ok) throw new Error(`Failed to confirm pill for patient ${did}`);
  return res.json();
}
