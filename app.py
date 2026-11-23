
import streamlit as st
import subprocess
import json
from pathlib import Path
import pandas as pd
import os

# Page configuration
st.set_page_config(
    page_title="Smart Cache Simulator",
    page_icon="💻",
    layout="wide"
)


# Custom dashboard styling
st.markdown("""
<style>

/* Main background */
.stApp {
    background: linear-gradient(135deg, #0B1020 0%, #151B35 55%, #211640 100%);
    color: #F8FAFC;
}

/* Main content */
.block-container {
    padding-top: 2rem;
    padding-bottom: 3rem;
}

/* Main title */
h1 {
    color: #FFFFFF !important;
    font-size: 2.5rem !important;
    font-weight: 800 !important;
    letter-spacing: -1px;
}

/* Section headings */
h2, h3 {
    color: #C4B5FD !important;
    font-weight: 700 !important;
}

/* Paragraphs and labels */
p, label, .stMarkdown {
    color: #E2E8F0;
}

/* Input boxes */
.stTextInput input,
.stNumberInput input {
    background-color: #202B49 !important;
    color: white !important;
    border: 1px solid #53658F !important;
    border-radius: 10px !important;
}

/* Dropdown */
.stSelectbox div[data-baseweb="select"] {
    background-color: #202B49;
    border-radius: 10px;
}

/* Buttons */
.stButton > button {
    background: linear-gradient(90deg, #2563EB, #7C3AED);
    color: white !important;
    border: none;
    border-radius: 12px;
    padding: 0.6rem 1.5rem;
    font-weight: 700;
    transition: all 0.25s ease;
}

/* Button hover */
.stButton > button:hover {
    transform: translateY(-3px);
    box-shadow: 0 6px 22px rgba(99, 102, 241, 0.45);
    border: none;
}

/* Metric cards */
div[data-testid="stMetric"] {
    background: linear-gradient(135deg, #202B49, #30245B);
    border: 1px solid #51438B;
    padding: 20px;
    border-radius: 16px;
    box-shadow: 0 5px 20px rgba(0,0,0,0.2);
}

/* Metric values */
div[data-testid="stMetricValue"] {
    color: #A5B4FC !important;
    font-weight: 800;
}

/* Tables */
div[data-testid="stDataFrame"] {
    border: 1px solid #465477;
    border-radius: 12px;
    overflow: hidden;
}

/* Success messages */
div[data-testid="stAlert"] {
    border-radius: 12px;
}

/* Dividers */
hr {
    border-color: #394566;
}

/* Smooth animation */
h1, h2, h3 {
    animation: fadeIn 0.6s ease-in-out;
}

@keyframes fadeIn {
    from {
        opacity: 0;
        transform: translateY(8px);
    }
    to {
        opacity: 1;
        transform: translateY(0);
    }
}

</style>
""", unsafe_allow_html=True)


# Connect Python to the C++ backend

# Connect Python to the C++ backend
def run_cpp_backend(capacity, requests):

    input_data = (
        f"{capacity} {len(requests)} "
        + " ".join(map(str, requests))
    )

    project_folder = Path(__file__).parent

    # Compile the C++ backend if needed
    executable = project_folder / (
        "cache_backend.exe" if os.name == "nt" else "cache_backend"
    )

    if not executable.exists():
        compile_result = subprocess.run(
            [
                "g++",
                str(project_folder / "cache_backend.cpp"),
                "-o",
                str(executable)
            ],
            capture_output=True,
            text=True
        )

        if compile_result.returncode != 0:
            raise RuntimeError(compile_result.stderr)

    result = subprocess.run(
        [str(executable)],
        input=input_data,
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        raise RuntimeError(result.stderr)

    data = json.loads(result.stdout)

    return data["results"]



def generate_trace(policy, capacity, requests):
    cache = []
    frequency = {}
    hits = 0
    misses = 0
    steps = []

    for i, page in enumerate(requests):
        removed = "-"
        status = "MISS"

        if page in cache:
            status = "HIT"
            hits += 1

            if policy == "LRU":
                cache.remove(page)
                cache.insert(0, page)

            elif policy == "LFU":
                frequency[page] += 1
                cache.remove(page)
                cache.insert(0, page)

        else:
            misses += 1

            if len(cache) == capacity:
                if policy == "LRU":
                    victim = cache[-1]

                elif policy == "FIFO":
                    victim = cache[-1]

                elif policy == "LFU":
                    min_freq = min(frequency[p] for p in cache)
                    victim = next(
                        p for p in reversed(cache)
                        if frequency[p] == min_freq
                    )

                else:  # Optimal
                    def next_use(p):
                        try:
                            return requests.index(p, i + 1)
                        except ValueError:
                            return float("inf")

                    victim = max(
                        cache,
                        key=lambda p: (next_use(p), -p)
                    )

                cache.remove(victim)
                del frequency[victim]
                removed = victim

            if capacity > 0:
                cache.insert(0, page)
                frequency[page] = 1

        steps.append({
            "Step": i + 1,
            "Request": page,
            "Status": status,
            "Evicted": removed,
            "Cache": cache.copy(),
            "Hits": hits,
            "Misses": misses
        })

    return steps

# Dashboard heading
st.title("💻 Smart Cache Management Simulator")
st.caption("Explore and compare cache replacement algorithms")

st.divider()

# Input section
st.header("Simulation Configuration")

col1, col2 = st.columns(2)

with col1:
    capacity = st.number_input(
        "Cache Capacity",
        min_value=1,
        value=3,
        step=1
    )



with col2:
    request_input = st.text_input(
        "Request Sequence",
        value="1, 2, 3, 1, 4, 1, 5, 1, 2, 3"
    )

selected_policy = st.selectbox(
    "Select Replacement Policy",
    ["Compare All", "LRU", "FIFO", "LFU", "Optimal"]
)

# Run simulation
if st.button("🚀 Run Simulation", type="primary"):

    try:
        requests = [
            int(x.strip())
            for x in request_input.split(",")
            if x.strip()
        ]

        if not requests:
            st.error("Please enter a request sequence.")

        elif any(x < 0 for x in requests):
            st.error("Requests cannot be negative.")

        else:
            results = run_cpp_backend(capacity, requests)

            st.session_state.simulation = {
                "capacity": capacity,
                "requests": requests,
                "results": results
            }

            st.session_state.step_index = 0

    except ValueError:
        st.error("Enter valid numbers separated by commas.")

    except Exception as e:
        st.error(f"Simulation error: {e}")


# Display saved simulation results
if "simulation" in st.session_state:

    simulation = st.session_state.simulation
    results = simulation["results"]
    requests = simulation["requests"]
    capacity = simulation["capacity"]
    requests = simulation["requests"]
    capacity = simulation["capacity"]

    st.success("Simulation completed successfully!")

    # Performance cards
    st.header("📊 Performance Overview")

    real_policies = [
        r for r in results if r["name"] != "Optimal"
    ]

    best_ratio = max(r["hitRatio"] for r in real_policies)

    best_names = [
        r["name"] for r in real_policies
        if r["hitRatio"] == best_ratio
    ]

    optimal = next(
        r for r in results if r["name"] == "Optimal"
    )

    col1, col2, col3 = st.columns(3)

    col1.metric("Best Real Policy", " / ".join(best_names))
    col2.metric("Best Real Hit Ratio", f"{best_ratio:.2f}%")
    col3.metric("Optimal Hit Ratio", f'{optimal["hitRatio"]:.2f}%')

    # Comparison chart
    st.header("📈 Hit Ratio Comparison")

    chart_df = pd.DataFrame(results).set_index("name")[["hitRatio"]]
    st.bar_chart(chart_df)

    # Comparison table
    st.header("📋 Algorithm Comparison")

    df = pd.DataFrame(results)
    df["hitRatio"] = df["hitRatio"].map(lambda x: f"{x:.2f}%")
    df["missRatio"] = df["missRatio"].map(lambda x: f"{x:.2f}%")

    st.dataframe(df, use_container_width=True)

    
    # Step-by-step visualization
    st.divider()
    st.header("🎬 Step-by-Step Cache Visualization")

    # Compare All is for the comparison dashboard.
    # Use LRU as the default policy for the step-by-step view.
    trace_policy = "LRU" if selected_policy == "Compare All" else selected_policy

    trace = generate_trace(trace_policy, capacity, requests)

    if "step_index" not in st.session_state:
        st.session_state.step_index = 0

    # Keep the step index within valid limits
    current_step = min(st.session_state.step_index, len(trace) - 1)
    current = trace[current_step]

    st.subheader(f"Step {current_step + 1} of {len(trace)}")

    progress = (current_step + 1) / len(trace)
    st.progress(progress)

    st.caption(f"{current_step + 1} of {len(trace)} requests processed")

    col1, col2, col3 = st.columns(3)

    col1.metric("Current Request", current["Request"])
    col2.metric("Status", current["Status"])
    col3.metric("Evicted Page", current["Evicted"])

    st.write(f"**Selected Policy:** {trace_policy}")

    # Live cache memory
    st.write("### 🧊 Live Cache Memory")

    cache_columns = st.columns(capacity)

    for i in range(capacity):
        with cache_columns[i]:

            if i < len(current["Cache"]):
                page = current["Cache"][i]

                is_new = (
                    current["Status"] == "MISS"
                    and page == current["Request"]
                )

                if is_new:
                    bg = "#2563EB"
                    border = "#60A5FA"
                    label = "NEW PAGE"
                else:
                    bg = "#202B49"
                    border = "#8B5CF6"
                    label = "STORED"

                st.markdown(
                    f"""
                    <div style="
                        background: {bg};
                        border: 2px solid {border};
                        border-radius: 15px;
                        padding: 24px 8px;
                        text-align: center;
                        box-shadow: 0 0 14px {border}55;
                    ">
                        <div style="color:#DDD6FE;font-size:12px;">
                            {label}
                        </div>
                        <div style="
                            color:white;
                            font-size:30px;
                            font-weight:800;
                        ">
                            {page}
                        </div>
                    </div>
                    """,
                    unsafe_allow_html=True
                )

            else:
                st.markdown(
                    """
                    <div style="
                        background:#151B35;
                        border:2px dashed #53658F;
                        border-radius:15px;
                        padding:24px 8px;
                        text-align:center;
                        color:#94A3B8;
                    ">
                        <div style="font-size:26px;">＋</div>
                        EMPTY
                    </div>
                    """,
                    unsafe_allow_html=True
                )

    # Hit / Miss indicator
    if current["Status"] == "HIT":
        st.success(
            f"🟢 HIT! Page {current['Request']} was already in cache."
        )
    else:
        st.error(
            f"🔴 MISS! Page {current['Request']} was not in cache."
        )

    if current["Evicted"] != "-":
        st.warning(
            f"♻️ Page {current['Evicted']} was evicted to make space."
        )

    # Navigation buttons
    previous_col, next_col = st.columns(2)

    with previous_col:
        if st.button(
            "⬅ Previous",
            disabled=(current_step == 0)
        ):
            st.session_state.step_index -= 1
            st.rerun()

    with next_col:
        if st.button(
            "Next Step ➡",
            disabled=(current_step == len(trace) - 1)
        ):
            st.session_state.step_index += 1
            st.rerun()

    # Request history
    st.subheader("📋 Request History")

    history_df = pd.DataFrame(trace[:current_step + 1])
    st.dataframe(history_df, use_container_width=True)
