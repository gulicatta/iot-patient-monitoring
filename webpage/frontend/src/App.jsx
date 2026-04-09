import { useEffect, useState } from "react";
import {
  Alert,
  AppBar,
  Box,
  Chip,
  CircularProgress,
  Container,
  Toolbar,
  Typography,
} from "@mui/material";
import LocalHospitalIcon from "@mui/icons-material/LocalHospital";
import {
  fetchHealth,
  fetchPatient,
  fetchPatients,
  fetchPillStatus,
  fetchConfig,
  confirmPill,
} from "./api";
import PatientOverview from "./components/PatientOverview";
import PatientDetail from "./components/PatientDetail";

function App() {
  const [health, setHealth] = useState(null);
  const [patients, setPatients] = useState([]);
  const [selectedDid, setSelectedDid] = useState(null);
  const [patientDetail, setPatientDetail] = useState(null);
  const [pillStatuses, setPillStatuses] = useState({});
  const [config, setConfig] = useState(null);
  const [error, setError] = useState(null);
  const [loading, setLoading] = useState(true);

  // Initial load
  useEffect(() => {
    Promise.all([fetchHealth(), fetchPatients(), fetchConfig().catch(() => null)])
      .then(([h, p, cfg]) => {
        setHealth(h);
        setPatients(p);
        if (cfg) setConfig(cfg);
        // Fetch pill status for all known patients (silently ignore if endpoint not ready)
        p.forEach((patient) => {
          fetchPillStatus(patient.id)
            .then((s) => setPillStatuses((prev) => ({ ...prev, [patient.id]: s })))
            .catch(() => {});
        });
      })
      .catch((err) => setError(err.message))
      .finally(() => setLoading(false));
  }, []);

  // 2-second polling: refresh patient list + selected patient detail + pill statuses
  useEffect(() => {
    const id = setInterval(() => {
      fetchPatients()
        .then((p) => {
          setPatients(p);
          // Keep pill statuses fresh for all patients
          p.forEach((patient) => {
            fetchPillStatus(patient.id)
              .then((s) => setPillStatuses((prev) => ({ ...prev, [patient.id]: s })))
              .catch(() => {});
          });
        })
        .catch((err) => setError(err.message));

      if (selectedDid !== null) {
        fetchPatient(selectedDid)
          .then(setPatientDetail)
          .catch((err) => setError(err.message));
      }
    }, 2000);
    return () => clearInterval(id);
  }, [selectedDid]);

  // Immediate detail load when selected patient changes
  useEffect(() => {
    if (selectedDid === null) {
      setPatientDetail(null);
      return;
    }
    fetchPatient(selectedDid)
      .then(setPatientDetail)
      .catch((err) => setError(err.message));
  }, [selectedDid]);

  function handleConfirmPill(did) {
    confirmPill(did)
      .then((s) => setPillStatuses((prev) => ({ ...prev, [did]: s })))
      .catch((err) => setError(err.message));
  }

  return (
    <>
      <AppBar position="static">
        <Toolbar>
          <LocalHospitalIcon sx={{ mr: 1.5, opacity: 0.85 }} />
          <Typography variant="h6" sx={{ flexGrow: 1, fontWeight: 600, letterSpacing: 0.3 }}>
            Patient Monitor
          </Typography>
          {health && (
            <Chip
              label={`Backend: ${health.status}`}
              color="success"
              size="small"
            />
          )}
        </Toolbar>
      </AppBar>

      <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
        {error && (
          <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
            {error}
          </Alert>
        )}

        {loading ? (
          <Box sx={{ display: "flex", justifyContent: "center", mt: 8 }}>
            <CircularProgress />
          </Box>
        ) : (
          <>
            <PatientOverview
              patients={patients}
              selectedDid={selectedDid}
              onSelect={setSelectedDid}
              pillStatuses={pillStatuses}
              tempThresholds={config?.skin_temperature}
            />
            <PatientDetail
              did={selectedDid}
              detail={patientDetail}
              pillStatus={pillStatuses[selectedDid]}
              onConfirmPill={handleConfirmPill}
              tempThresholds={config?.skin_temperature}
            />
          </>
        )}
      </Container>
    </>
  );
}

export default App;
