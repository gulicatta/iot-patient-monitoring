import { createTheme } from "@mui/material/styles";

const theme = createTheme({
  palette: {
    primary: { main: "#1a3c5e" },
    warning: { main: "#f5c518", contrastText: "#000000" },
    background: { default: "#f4f6f9", paper: "#ffffff" },
  },
  shape: { borderRadius: 8 },
  typography: {
    fontFamily: '"Inter", "Roboto", "Helvetica Neue", Arial, sans-serif',
  },
  components: {
    MuiPaper: {
      defaultProps: { elevation: 0 },
      styleOverrides: { root: { border: "1px solid #e2e8f0" } },
    },
    MuiAppBar: {
      styleOverrides: {
        root: { boxShadow: "none", borderBottom: "1px solid rgba(18, 50, 80, 0.25)" },
      },
    },
    MuiTableRow: {
      styleOverrides: {
        root: {
          "&.Mui-selected": {
            backgroundColor: "#e8f0fb",
            "&:hover": { backgroundColor: "#dce8f8" },
          },
        },
      },
    },
    MuiChip: { styleOverrides: { root: { borderRadius: 6 } } },
    MuiButton: { styleOverrides: { root: { textTransform: "none" } } },
  },
});

export default theme;
