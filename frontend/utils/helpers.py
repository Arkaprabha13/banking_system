import streamlit as st
from typing import Dict, Any
import requests

class AuthManager:
    """Handle authentication and session management"""
    
    @staticmethod
    def login(username: str, password: str) -> Dict[str, Any]:
        """Authenticate user with backend"""
        try:
            response = requests.post(
                "http://localhost:8080/api/auth/login",
                json={"username": username, "password": password},
                timeout=10
            )
            
            if response.status_code == 200:
                data = response.json()
                if data.get("success"):
                    # Store session data
                    st.session_state.logged_in = True
                    st.session_state.user_id = data.get("user_id")
                    st.session_state.username = username
                    st.session_state.role = data.get("role", "CUSTOMER")
                    return {"success": True, "message": "Login successful"}
                else:
                    return {"success": False, "error": data.get("message", "Login failed")}
            else:
                return {"success": False, "error": f"Server error: {response.status_code}"}
                
        except requests.exceptions.ConnectionError:
            return {"success": False, "error": "Cannot connect to server. Please ensure the backend is running."}
        except requests.exceptions.Timeout:
            return {"success": False, "error": "Request timeout. Please try again."}
        except Exception as e:
            return {"success": False, "error": f"Unexpected error: {str(e)}"}
    
    @staticmethod
    def register(username: str, password: str, email: str, full_name: str, phone: str = "") -> Dict[str, Any]:
        """Register new user"""
        try:
            response = requests.post(
                "http://localhost:8080/api/auth/register",
                json={
                    "username": username,
                    "password": password,
                    "email": email,
                    "full_name": full_name,
                    "phone": phone
                },
                timeout=10
            )
            
            if response.status_code == 200:
                data = response.json()
                return data
            else:
                return {"success": False, "error": f"Server error: {response.status_code}"}
                
        except requests.exceptions.ConnectionError:
            return {"success": False, "error": "Cannot connect to server"}
        except Exception as e:
            return {"success": False, "error": str(e)}
    
    @staticmethod
    def logout():
        """Clear session data"""
        for key in ['logged_in', 'user_id', 'username', 'role']:
            if key in st.session_state:
                del st.session_state[key]
    
    @staticmethod
    def is_authenticated() -> bool:
        """Check if user is authenticated"""
        return st.session_state.get('logged_in', False)
    
    @staticmethod
    def get_current_user() -> Dict[str, Any]:
        """Get current user information"""
        return {
            "user_id": st.session_state.get('user_id'),
            "username": st.session_state.get('username'),
            "role": st.session_state.get('role')
        }

class Validator:
    """Input validation utilities"""
    
    @staticmethod
    def validate_username(username: str) -> Dict[str, Any]:
        """Validate username"""
        if not username:
            return {"valid": False, "error": "Username is required"}
        if len(username) < 3:
            return {"valid": False, "error": "Username must be at least 3 characters"}
        if len(username) > 20:
            return {"valid": False, "error": "Username must be less than 20 characters"}
        if not username.isalnum():
            return {"valid": False, "error": "Username can only contain letters and numbers"}
        return {"valid": True}
    
    @staticmethod
    def validate_password(password: str) -> Dict[str, Any]:
        """Validate password"""
        if not password:
            return {"valid": False, "error": "Password is required"}
        if len(password) < 6:
            return {"valid": False, "error": "Password must be at least 6 characters"}
        if len(password) > 50:
            return {"valid": False, "error": "Password must be less than 50 characters"}
        return {"valid": True}
    
    @staticmethod
    def validate_email(email: str) -> Dict[str, Any]:
        """Validate email address"""
        if not email:
            return {"valid": False, "error": "Email is required"}
        if "@" not in email or "." not in email:
            return {"valid": False, "error": "Please enter a valid email address"}
        if len(email) > 100:
            return {"valid": False, "error": "Email must be less than 100 characters"}
        return {"valid": True}
    
    @staticmethod
    def validate_amount(amount: float) -> Dict[str, Any]:
        """Validate transaction amount"""
        if amount <= 0:
            return {"valid": False, "error": "Amount must be greater than 0"}
        if amount > 1000000:
            return {"valid": False, "error": "Amount cannot exceed $1,000,000"}
        return {"valid": True}
    
    @staticmethod
    def validate_full_name(name: str) -> Dict[str, Any]:
        """Validate full name"""
        if not name:
            return {"valid": False, "error": "Full name is required"}
        if len(name) < 2:
            return {"valid": False, "error": "Full name must be at least 2 characters"}
        if len(name) > 100:
            return {"valid": False, "error": "Full name must be less than 100 characters"}
        return {"valid": True}

class UIHelpers:
    """UI utility functions"""
    
    @staticmethod
    def show_success(message: str):
        """Show success message"""
        st.success(f"✅ {message}")
    
    @staticmethod
    def show_error(message: str):
        """Show error message"""
        st.error(f"❌ {message}")
    
    @staticmethod
    def show_warning(message: str):
        """Show warning message"""
        st.warning(f"⚠️ {message}")
    
    @staticmethod
    def show_info(message: str):
        """Show info message"""
        st.info(f"ℹ️ {message}")
    
    @staticmethod
    def format_currency(amount: float) -> str:
        """Format amount as currency"""
        return f"${amount:,.2f}"
    
    @staticmethod
    def format_account_number(account_number: str) -> str:
        """Format account number for display"""
        if len(account_number) >= 8:
            return f"****{account_number[-4:]}"
        return account_number
    
    @staticmethod
    def get_account_type_icon(account_type: str) -> str:
        """Get icon for account type"""
        icons = {
            "CHECKING": "🏦",
            "SAVINGS": "💰",
            "BUSINESS": "🏢"
        }
        return icons.get(account_type.upper(), "💳")
