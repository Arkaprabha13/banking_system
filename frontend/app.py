import streamlit as st
import requests
import json
from typing import Dict, Any, Optional
from datetime import datetime
import pandas as pd
import time

# Configuration
API_BASE_URL = "http://localhost:8080"

class BankingAPI:
    """API client for communicating with the C++ backend"""
    
    @staticmethod
    def test_connection() -> Dict[str, Any]:
        """Test connection to backend server"""
        try:
            response = requests.get(f"{API_BASE_URL}/api", timeout=5)
            if response.status_code == 200:
                return {"success": True, "message": "Backend connected"}
            else:
                return {"success": False, "error": f"Backend returned status {response.status_code}"}
        except requests.exceptions.ConnectionError:
            return {"success": False, "error": "Cannot connect to backend server. Make sure the C++ server is running on port 8080."}
        except Exception as e:
            return {"success": False, "error": f"Connection test failed: {str(e)}"}
    
    @staticmethod
    def make_request(endpoint: str, method: str = "GET", data: Dict = None, params: Dict = None) -> Dict[str, Any]:
        """Make HTTP request to the backend API with detailed error handling"""
        try:
            url = f"{API_BASE_URL}{endpoint}"
            headers = {"Content-Type": "application/json"}
            
            st.write(f"🔍 **Debug:** Making {method} request to {url}")
            if data:
                st.write(f"📤 **Debug:** Request data: {json.dumps(data, indent=2)}")
            
            if method == "GET":
                response = requests.get(url, params=params, timeout=10)
            elif method == "POST":
                response = requests.post(url, json=data, headers=headers, timeout=10)
            elif method == "PUT":
                response = requests.put(url, json=data, headers=headers, timeout=10)
            else:
                return {"success": False, "error": "Unsupported HTTP method"}
            
            st.write(f"📥 **Debug:** Response status: {response.status_code}")
            st.write(f"📥 **Debug:** Response text: {response.text}")
            
            # Try to parse JSON response
            try:
                response_data = response.json()
            except json.JSONDecodeError:
                response_data = {"raw_response": response.text}
            
            if response.status_code == 200:
                return response_data
            else:
                return {
                    "success": False, 
                    "error": f"HTTP {response.status_code}", 
                    "details": response_data
                }
                
        except requests.exceptions.ConnectionError:
            return {"success": False, "error": "Cannot connect to backend server. Please check if the C++ server is running."}
        except requests.exceptions.Timeout:
            return {"success": False, "error": "Request timeout - server took too long to respond"}
        except Exception as e:
            return {"success": False, "error": f"Request failed: {str(e)}"}
    
    @staticmethod
    def login(username: str, password: str) -> Dict[str, Any]:
        """Authenticate user - Fixed endpoint"""
        return BankingAPI.make_request("/api/login", "POST", {
            "username": username,
            "password": password
        })
    
    @staticmethod
    def register(username: str, password: str, email: str, full_name: str, phone: str = "") -> Dict[str, Any]:
        """Register new user - Fixed endpoint"""
        return BankingAPI.make_request("/api/register", "POST", {
            "username": username,
            "password": password,
            "email": email,
            "full_name": full_name,
            "phone": phone
        })
    
    @staticmethod
    def get_accounts(username: str) -> Dict[str, Any]:
        """Get user accounts - Fixed to use query parameter"""
        return BankingAPI.make_request("/api/accounts", "GET", params={"username": username})
    
    @staticmethod
    def create_account(username: str, account_type: str, initial_balance: float = 0.0) -> Dict[str, Any]:
        """Create new account - Fixed parameters"""
        return BankingAPI.make_request("/api/accounts/create", "POST", {
            "username": username,
            "account_type": account_type,
            "initial_balance": initial_balance
        })
    
    @staticmethod
    def deposit(account_number: str, amount: float, description: str = "Deposit") -> Dict[str, Any]:
        """Make deposit"""
        return BankingAPI.make_request("/api/transactions/deposit", "POST", {
            "account_number": account_number,
            "amount": amount,
            "description": description
        })
    
    @staticmethod
    def withdraw(account_number: str, amount: float, description: str = "Withdrawal") -> Dict[str, Any]:
        """Make withdrawal"""
        return BankingAPI.make_request("/api/transactions/withdraw", "POST", {
            "account_number": account_number,
            "amount": amount,
            "description": description
        })
    
    @staticmethod
    def transfer(from_account: str, to_account: str, amount: float, description: str = "Transfer") -> Dict[str, Any]:
        """Make transfer"""
        return BankingAPI.make_request("/api/transactions/transfer", "POST", {
            "from_account": from_account,
            "to_account": to_account,
            "amount": amount,
            "description": description
        })
    
    @staticmethod
    def get_transactions(account_number: str) -> Dict[str, Any]:
        """Get account transactions - Fixed to use query parameter"""
        return BankingAPI.make_request("/api/transactions", "GET", params={"account_number": account_number})
    
    @staticmethod
    def get_balance(account_number: str) -> Dict[str, Any]:
        """Get account balance - Fixed to use query parameter"""
        return BankingAPI.make_request("/api/balance", "GET", params={"account_number": account_number})

# Session state management
def init_session_state():
    """Initialize session state variables"""
    if 'logged_in' not in st.session_state:
        st.session_state.logged_in = False
    if 'user_id' not in st.session_state:
        st.session_state.user_id = None
    if 'username' not in st.session_state:
        st.session_state.username = None
    if 'role' not in st.session_state:
        st.session_state.role = "CUSTOMER"
    if 'show_register' not in st.session_state:
        st.session_state.show_register = False
    if 'debug_mode' not in st.session_state:
        st.session_state.debug_mode = True

def logout():
    """Clear session state"""
    st.session_state.logged_in = False
    st.session_state.user_id = None
    st.session_state.username = None
    st.session_state.role = "CUSTOMER"

def show_connection_status():
    """Show backend connection status"""
    st.sidebar.markdown("### 🔌 Backend Status")
    
    if st.sidebar.button("Test Connection"):
        with st.sidebar:
            with st.spinner("Testing connection..."):
                result = BankingAPI.test_connection()
            
            if result.get("success"):
                st.success("✅ Backend Connected")
            else:
                st.error(f"❌ {result.get('error')}")

# Authentication pages
def login_page():
    """Login page with improved error handling"""
    st.title("🏦 Banking System Login")
    
    # Show connection status
    show_connection_status()
    
    col1, col2, col3 = st.columns([1, 2, 1])
    
    with col2:
        st.markdown("### Sign In to Your Account")
        
        # Test credentials info
        st.info("**Test Credentials:** Username: `admin`, Password: `password`")
        
        with st.form("login_form"):
            username = st.text_input("Username", placeholder="Enter your username", value="admin")
            password = st.text_input("Password", type="password", placeholder="Enter your password", value="password")
            
            col_login, col_register = st.columns(2)
            
            with col_login:
                login_submitted = st.form_submit_button("Login", use_container_width=True)
            
            with col_register:
                if st.form_submit_button("Register New Account", use_container_width=True):
                    st.session_state.show_register = True
                    st.rerun()
        
        if login_submitted:
            if username and password:
                with st.spinner("Authenticating..."):
                    result = BankingAPI.login(username, password)
                
                st.write("**API Response:**")
                st.json(result)
                
                if result.get("success"):
                    st.session_state.logged_in = True
                    st.session_state.user_id = result.get("user_id", username)
                    st.session_state.username = username
                    st.session_state.role = result.get("role", "CUSTOMER")
                    st.success("✅ Login successful!")
                    time.sleep(1)
                    st.rerun()
                else:
                    st.error(f"❌ Login failed: {result.get('error', 'Unknown error')}")
                    if 'details' in result:
                        st.write("**Error Details:**")
                        st.json(result['details'])
            else:
                st.error("Please enter both username and password")

def register_page():
    """Registration page with improved validation"""
    st.title("🏦 Banking System Registration")
    
    show_connection_status()
    
    col1, col2, col3 = st.columns([1, 2, 1])
    
    with col2:
        st.markdown("### Create New Account")
        
        with st.form("register_form"):
            username = st.text_input("Username", placeholder="Choose a username (min 3 characters)")
            password = st.text_input("Password", type="password", placeholder="Choose a password (min 6 characters)")
            confirm_password = st.text_input("Confirm Password", type="password", placeholder="Confirm your password")
            email = st.text_input("Email", placeholder="Enter your email address")
            full_name = st.text_input("Full Name", placeholder="Enter your full name")
            phone = st.text_input("Phone Number", placeholder="Enter your phone number (optional)")
            
            col_register, col_back = st.columns(2)
            
            with col_register:
                register_submitted = st.form_submit_button("Register", use_container_width=True)
            
            with col_back:
                if st.form_submit_button("Back to Login", use_container_width=True):
                    st.session_state.show_register = False
                    st.rerun()
        
        if register_submitted:
            # Validation
            if not all([username, password, email, full_name]):
                st.error("Please fill in all required fields")
            elif len(username) < 3:
                st.error("Username must be at least 3 characters long")
            elif len(password) < 6:
                st.error("Password must be at least 6 characters long")
            elif password != confirm_password:
                st.error("Passwords do not match")
            elif "@" not in email:
                st.error("Please enter a valid email address")
            else:
                with st.spinner("Creating account..."):
                    result = BankingAPI.register(username, password, email, full_name, phone)
                
                st.write("**API Response:**")
                st.json(result)
                
                if result.get("success"):
                    st.success("✅ Registration successful! You can now login.")
                    st.session_state.show_register = False
                    time.sleep(2)
                    st.rerun()
                else:
                    st.error(f"❌ Registration failed: {result.get('error', 'Unknown error')}")

# Main dashboard
def dashboard():
    """Main dashboard after login"""
    st.title(f"🏦 Welcome, {st.session_state.username}!")
    
    # Sidebar navigation
    with st.sidebar:
        st.markdown(f"**Logged in as:** {st.session_state.username}")
        st.markdown(f"**Role:** {st.session_state.role}")
        st.markdown("---")
        
        page = st.selectbox("Navigation", [
            "Dashboard",
            "My Accounts", 
            "Transactions",
            "Transfer Money",
            "Account Management",
            "API Testing"
        ])
        
        st.markdown("---")
        show_connection_status()
        
        st.markdown("---")
        if st.button("Logout", use_container_width=True):
            logout()
            st.rerun()
    
    # Main content based on selected page
    if page == "Dashboard":
        show_dashboard_overview()
    elif page == "My Accounts":
        show_accounts()
    elif page == "Transactions":
        show_transactions()
    elif page == "Transfer Money":
        show_transfer()
    elif page == "Account Management":
        show_account_management()
    elif page == "API Testing":
        show_api_testing()

def show_dashboard_overview():
    """Dashboard overview page with better error handling"""
    st.markdown("### Account Overview")
    
    # Get user accounts
    with st.spinner("Loading accounts..."):
        accounts_result = BankingAPI.get_accounts(st.session_state.username)
    
    st.write("**API Response:**")
    st.json(accounts_result)
    
    if accounts_result.get("success") is False:
        st.error(f"Failed to load accounts: {accounts_result.get('error')}")
        return
    
    accounts = accounts_result.get("accounts", [])
    
    if accounts:
        # Display account cards
        cols = st.columns(min(len(accounts), 3))
        
        for idx, account in enumerate(accounts):
            with cols[idx % 3]:
                with st.container():
                    st.markdown(f"""
                    <div style="
                        border: 1px solid #ddd;
                        border-radius: 10px;
                        padding: 20px;
                        margin: 10px 0;
                        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                        color: white;
                    ">
                        <h4>{account.get('account_type', 'Unknown')} Account</h4>
                        <p><strong>Account #:</strong> {account.get('account_number', 'N/A')}</p>
                        <h3>${float(account.get('balance', 0)):,.2f}</h3>
                        <p><small>Status: {account.get('status', 'Unknown')}</small></p>
                    </div>
                    """, unsafe_allow_html=True)
        
        # Total balance
        total_balance = sum(float(account.get('balance', 0)) for account in accounts)
        st.markdown("---")
        st.metric("Total Balance", f"${total_balance:,.2f}")
        
    else:
        st.info("No accounts found. Create your first account!")
        if st.button("Create Account"):
            st.session_state.page = "My Accounts"
            st.rerun()

def show_accounts():
    """Accounts management page with improved error handling"""
    st.markdown("### My Accounts")
    
    # Get accounts
    with st.spinner("Loading accounts..."):
        accounts_result = BankingAPI.get_accounts(st.session_state.username)
    
    if st.checkbox("Show API Response"):
        st.json(accounts_result)
    
    if accounts_result.get("success") is False:
        st.error(f"Failed to load accounts: {accounts_result.get('error')}")
    else:
        accounts = accounts_result.get("accounts", [])
        
        if accounts:
            # Create DataFrame for display
            df_data = []
            for account in accounts:
                df_data.append({
                    "Account Number": account.get('account_number', 'N/A'),
                    "Type": account.get('account_type', 'Unknown'),
                    "Balance": f"${float(account.get('balance', 0)):,.2f}",
                    "Status": account.get('status', 'Unknown')
                })
            
            df = pd.DataFrame(df_data)
            st.dataframe(df, use_container_width=True)
        else:
            st.info("No accounts found")
    
    # Create new account section
    st.markdown("### Create New Account")
    
    with st.form("create_account_form"):
        account_type = st.selectbox("Account Type", ["CHECKING", "SAVINGS", "BUSINESS"])
        initial_balance = st.number_input("Initial Balance", min_value=0.0, value=0.0, step=10.0)
        
        if st.form_submit_button("Create Account"):
            with st.spinner("Creating account..."):
                result = BankingAPI.create_account(st.session_state.username, account_type, initial_balance)
            
            st.write("**API Response:**")
            st.json(result)
            
            if result.get("success"):
                st.success(f"✅ Account created successfully! Account Number: {result.get('account_number')}")
                time.sleep(2)
                st.rerun()
            else:
                st.error(f"❌ Failed to create account: {result.get('error')}")

def show_transactions():
    """Transaction history page"""
    st.markdown("### Transaction History")
    
    # Get accounts first
    accounts_result = BankingAPI.get_accounts(st.session_state.username)
    
    if accounts_result.get("success") is False:
        st.error("Failed to load accounts")
        return
    
    accounts = accounts_result.get("accounts", [])
    
    if accounts:
        account_options = {f"{acc.get('account_number', 'N/A')} ({acc.get('account_type', 'Unknown')})": acc.get('account_number', '') 
                         for acc in accounts}
        
        selected_account_display = st.selectbox("Select Account", list(account_options.keys()))
        selected_account = account_options[selected_account_display]
        
        if selected_account:
            # Get transactions
            with st.spinner("Loading transactions..."):
                transactions_result = BankingAPI.get_transactions(selected_account)
            
            if st.checkbox("Show Transaction API Response"):
                st.json(transactions_result)
            
            if transactions_result.get("success") is False:
                st.error(f"Failed to load transactions: {transactions_result.get('error')}")
            else:
                transactions = transactions_result.get("transactions", [])
                
                if transactions:
                    # Create DataFrame
                    df_data = []
                    for txn in transactions:
                        df_data.append({
                            "Date & Time": txn.get('timestamp', 'N/A'),
                            "Type": txn.get('type', 'Unknown'),
                            "Amount": f"${float(txn.get('amount', 0)):,.2f}",
                            "Description": txn.get('description', 'N/A'),
                            "Status": txn.get('status', 'Unknown')
                        })
                    
                    df = pd.DataFrame(df_data)
                    st.dataframe(df, use_container_width=True)
                else:
                    st.info("No transactions found for this account")
    else:
        st.info("No accounts found")

def show_transfer():
    """Money transfer page with improved error handling"""
    st.markdown("### Transfer Money")
    
    # Get user accounts
    accounts_result = BankingAPI.get_accounts(st.session_state.username)
    
    if accounts_result.get("success") is False:
        st.error("Failed to load accounts")
        return
    
    accounts = accounts_result.get("accounts", [])
    
    if len(accounts) >= 1:
        account_options = {f"{acc.get('account_number', 'N/A')} ({acc.get('account_type', 'Unknown')}) - ${float(acc.get('balance', 0)):,.2f}": acc.get('account_number', '') 
                         for acc in accounts}
        
        col1, col2 = st.columns(2)
        
        with col1:
            st.markdown("#### Deposit")
            with st.form("deposit_form"):
                deposit_account = st.selectbox("To Account", list(account_options.keys()), key="deposit_account")
                deposit_amount = st.number_input("Amount", min_value=0.01, step=10.0, key="deposit_amount")
                deposit_description = st.text_input("Description", value="Deposit", key="deposit_desc")
                
                if st.form_submit_button("Deposit"):
                    account_number = account_options[deposit_account]
                    if account_number:
                        with st.spinner("Processing deposit..."):
                            result = BankingAPI.deposit(account_number, deposit_amount, deposit_description)
                        
                        st.write("**API Response:**")
                        st.json(result)
                        
                        if result.get("success"):
                            st.success(f"✅ Deposit successful! New balance: ${result.get('new_balance', 0):,.2f}")
                            time.sleep(2)
                            st.rerun()
                        else:
                            st.error(f"❌ Deposit failed: {result.get('error')}")
        
        with col2:
            st.markdown("#### Withdrawal")
            with st.form("withdrawal_form"):
                withdrawal_account = st.selectbox("From Account", list(account_options.keys()), key="withdrawal_account")
                withdrawal_amount = st.number_input("Amount", min_value=0.01, step=10.0, key="withdrawal_amount")
                withdrawal_description = st.text_input("Description", value="Withdrawal", key="withdrawal_desc")
                
                if st.form_submit_button("Withdraw"):
                    account_number = account_options[withdrawal_account]
                    if account_number:
                        with st.spinner("Processing withdrawal..."):
                            result = BankingAPI.withdraw(account_number, withdrawal_amount, withdrawal_description)
                        
                        st.write("**API Response:**")
                        st.json(result)
                        
                        if result.get("success"):
                            st.success(f"✅ Withdrawal successful! New balance: ${result.get('new_balance', 0):,.2f}")
                            time.sleep(2)
                            st.rerun()
                        else:
                            st.error(f"❌ Withdrawal failed: {result.get('error')}")
        
        # Transfer between accounts
        if len(accounts) >= 2:
            st.markdown("#### Transfer Between Accounts")
            with st.form("transfer_form"):
                from_account = st.selectbox("From Account", list(account_options.keys()), key="from_account")
                to_account = st.selectbox("To Account", list(account_options.keys()), key="to_account")
                transfer_amount = st.number_input("Amount", min_value=0.01, step=10.0, key="transfer_amount")
                transfer_description = st.text_input("Description", value="Internal Transfer", key="transfer_desc")
                
                if st.form_submit_button("Transfer"):
                    from_account_number = account_options[from_account]
                    to_account_number = account_options[to_account]
                    
                    if from_account_number == to_account_number:
                        st.error("Cannot transfer to the same account")
                    elif from_account_number and to_account_number:
                        with st.spinner("Processing transfer..."):
                            result = BankingAPI.transfer(from_account_number, to_account_number, transfer_amount, transfer_description)
                        
                        st.write("**API Response:**")
                        st.json(result)
                        
                        if result.get("success"):
                            st.success(f"✅ Transfer successful!")
                            time.sleep(2)
                            st.rerun()
                        else:
                            st.error(f"❌ Transfer failed: {result.get('error')}")
    else:
        st.info("You need at least one account to make transactions")

def show_account_management():
    """Account management page"""
    st.markdown("### Account Management")
    
    # Get accounts
    accounts_result = BankingAPI.get_accounts(st.session_state.username)
    
    if accounts_result.get("success") is False:
        st.error("Failed to load accounts")
        return
    
    accounts = accounts_result.get("accounts", [])
    
    for account in accounts:
        with st.expander(f"{account.get('account_type', 'Unknown')} Account - {account.get('account_number', 'N/A')}"):
            col1, col2 = st.columns(2)
            
            with col1:
                st.write(f"**Account Number:** {account.get('account_number', 'N/A')}")
                st.write(f"**Type:** {account.get('account_type', 'Unknown')}")
                st.write(f"**Status:** {account.get('status', 'Unknown')}")
            
            with col2:
                st.write(f"**Balance:** ${float(account.get('balance', 0)):,.2f}")
                st.write(f"**Daily Limit:** ${float(account.get('daily_limit', 0)):,.2f}")
                st.write(f"**Minimum Balance:** ${float(account.get('minimum_balance', 0)):,.2f}")

def show_api_testing():
    """API testing page for debugging"""
    st.markdown("### API Testing & Debugging")
    
    st.markdown("#### Test Individual Endpoints")
    
    # Test connection
    if st.button("Test Backend Connection"):
        result = BankingAPI.test_connection()
        st.json(result)
    
    # Test login
    st.markdown("#### Test Login")
    with st.form("test_login"):
        test_username = st.text_input("Username", value="admin")
        test_password = st.text_input("Password", value="password")
        
        if st.form_submit_button("Test Login"):
            result = BankingAPI.login(test_username, test_password)
            st.json(result)
    
    # Test get accounts
    st.markdown("#### Test Get Accounts")
    if st.button("Test Get Accounts"):
        result = BankingAPI.get_accounts(st.session_state.username)
        st.json(result)
    
    # Raw API request
    st.markdown("#### Raw API Request")
    with st.form("raw_request"):
        endpoint = st.text_input("Endpoint", value="/api")
        method = st.selectbox("Method", ["GET", "POST"])
        raw_data = st.text_area("JSON Data (for POST)", value='{"username": "admin", "password": "password"}')
        
        if st.form_submit_button("Send Request"):
            try:
                data = json.loads(raw_data) if raw_data else None
                result = BankingAPI.make_request(endpoint, method, data)
                st.json(result)
            except json.JSONDecodeError:
                st.error("Invalid JSON data")

# Main application
def main():
    st.set_page_config(
        page_title="Banking System",
        page_icon="🏦",
        layout="wide",
        initial_sidebar_state="expanded"
    )
    
    # Custom CSS
    st.markdown("""
    <style>
    .main-header {
        text-align: center;
        color: #2E86AB;
        font-size: 2.5rem;
        margin-bottom: 2rem;
    }
    .metric-card {
        background: white;
        padding: 1rem;
        border-radius: 0.5rem;
        border: 1px solid #ddd;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    </style>
    """, unsafe_allow_html=True)
    
    init_session_state()
    
    # Check if showing register page
    if st.session_state.get('show_register', False):
        register_page()
    elif not st.session_state.logged_in:
        login_page()
    else:
        dashboard()

if __name__ == "__main__":
    main()
