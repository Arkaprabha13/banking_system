// Banking System Frontend JavaScript
const API_BASE = 'http://localhost:8080/api';

// Wait for Alpine to be ready
document.addEventListener('alpine:init', () => {
    console.log('Alpine.js initializing...');
    
    Alpine.data('bankingApp', () => ({
        // Authentication state
        isAuthenticated: false,
        currentUser: null,
        authMode: 'login',
        
        // Application state
        accounts: [],
        transactions: [],
        selectedAccount: null,
        activeTab: 'accounts',
        connectionStatus: null,
        
        // Modal states
        showCreateAccountModal: false,
        showDepositModal: false,
        showWithdrawModal: false,
        showTransferModal: false,
        
        // Form data
        loginForm: {
            username: 'admin',
            password: 'password'
        },
        registerForm: {
            username: '',
            password: '',
            full_name: '',
            email: '',
            phone: ''
        },
        createAccountForm: {
            account_type: 'SAVINGS',
            initial_balance: 0
        },
        depositForm: {
            amount: 0
        },
        withdrawForm: {
            amount: 0
        },
        transferForm: {
            from_account: '',
            to_account: '',
            amount: 0,
            description: ''
        },
        
        // Loading and error states
        loading: false,
        error: '',
        successMessage: '',
        
        // Computed properties
        get totalBalance() {
            return this.accounts.reduce((sum, account) => sum + (account.balance || 0), 0);
        },
        
        // Transaction type mapping - converts numbers to readable text
        getTransactionTypeText(typeNumber) {
            const typeMap = {
                0: 'Deposit',
                1: 'Withdrawal', 
                2: 'Transfer',
                3: 'Payment',
                4: 'Fee'
            };
            
            return typeMap[typeNumber] || typeMap[typeNumber?.toString()] || 'Unknown';
        },
        
        // Enhanced transaction type display with icons and colors
        getTransactionTypeDisplay(typeNumber) {
            const typeMap = {
                0: { text: 'Deposit', icon: '💰', class: 'text-green-600 bg-green-50' },
                1: { text: 'Withdrawal', icon: '💸', class: 'text-red-600 bg-red-50' },
                2: { text: 'Transfer', icon: '🔄', class: 'text-blue-600 bg-blue-50' },
                3: { text: 'Payment', icon: '💳', class: 'text-purple-600 bg-purple-50' },
                4: { text: 'Fee', icon: '📋', class: 'text-orange-600 bg-orange-50' }
            };
            
            return typeMap[typeNumber] || typeMap[typeNumber?.toString()] || 
                   { text: 'Unknown', icon: '❓', class: 'text-gray-600 bg-gray-50' };
        },
        
        // Initialize the app
        async init() {
            console.log('Banking app initializing...');
            await this.testConnection();
            this.checkAuthStatus();
        },
        
        // Test backend connection
        async testConnection() {
            console.log('Testing backend connection...');
            try {
                const response = await fetch(`${API_BASE}`, { 
                    method: 'GET',
                    mode: 'cors',
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });
                
                if (response.ok) {
                    this.connectionStatus = {
                        success: true,
                        message: 'Backend connected successfully'
                    };
                } else {
                    throw new Error(`HTTP ${response.status}`);
                }
            } catch (error) {
                console.error('Connection test failed:', error);
                this.connectionStatus = {
                    success: false,
                    message: 'Cannot connect to backend server. Make sure the C++ server is running on port 8080.'
                };
            }
        },
        
        // Check if user is already logged in
        checkAuthStatus() {
            const user = localStorage.getItem('currentUser');
            if (user) {
                try {
                    this.currentUser = JSON.parse(user);
                    this.isAuthenticated = true;
                    this.loadUserData();
                } catch (error) {
                    console.error('Invalid stored user data:', error);
                    localStorage.removeItem('currentUser');
                }
            }
        },
        
        // API helper function
        async apiCall(endpoint, options = {}) {
            this.loading = true;
            this.error = '';
            
            console.log(`Making API call to: ${API_BASE}${endpoint}`);
            
            try {
                const response = await fetch(`${API_BASE}${endpoint}`, {
                    mode: 'cors',
                    headers: {
                        'Content-Type': 'application/json',
                        ...options.headers
                    },
                    ...options
                });
                
                console.log(`Response status: ${response.status}`);
                
                let data;
                try {
                    data = await response.json();
                } catch (parseError) {
                    console.error('Failed to parse response as JSON:', parseError);
                    throw new Error('Invalid response format from server');
                }
                
                console.log('Response data:', data);
                
                if (!response.ok) {
                    throw new Error(data.message || data.error || `HTTP ${response.status}`);
                }
                
                return data;
            } catch (error) {
                console.error('API call failed:', error);
                this.error = error.message;
                throw error;
            } finally {
                this.loading = false;
            }
        },
        
        // Authentication methods
        async login() {
            console.log('Attempting login...');
            try {
                const response = await this.apiCall('/login', {
                    method: 'POST',
                    body: JSON.stringify(this.loginForm)
                });
                
                if (response.success) {
                    this.currentUser = { 
                        username: response.username,
                        user_id: response.user_id
                    };
                    this.isAuthenticated = true;
                    this.successMessage = 'Login successful!';
                    
                    localStorage.setItem('currentUser', JSON.stringify(this.currentUser));
                    
                    // Load user data
                    await this.loadUserData();
                    
                    // Clear success message after 3 seconds
                    setTimeout(() => this.successMessage = '', 3000);
                } else {
                    throw new Error(response.message || 'Login failed');
                }
                
            } catch (error) {
                console.error('Login failed:', error);
            }
        },
        
        async register() {
            console.log('Attempting registration...');
            try {
                const response = await this.apiCall('/register', {
                    method: 'POST',
                    body: JSON.stringify(this.registerForm)
                });
                
                if (response.success) {
                    this.successMessage = 'Registration successful! Please login.';
                    this.authMode = 'login';
                    
                    // Clear form
                    this.registerForm = { username: '', password: '', full_name: '', email: '', phone: '' };
                    
                    setTimeout(() => this.successMessage = '', 3000);
                } else {
                    throw new Error(response.message || 'Registration failed');
                }
                
            } catch (error) {
                console.error('Registration failed:', error);
            }
        },
        
        logout() {
            this.isAuthenticated = false;
            this.currentUser = null;
            this.accounts = [];
            this.transactions = [];
            this.selectedAccount = null;
            this.activeTab = 'accounts';
            localStorage.removeItem('currentUser');
            this.successMessage = 'Logged out successfully!';
            setTimeout(() => this.successMessage = '', 3000);
        },
        
        // Data loading methods
        async loadUserData() {
            if (!this.isAuthenticated) return;
            
            console.log('Loading user data...');
            try {
                await this.loadAccounts();
                if (this.accounts.length > 0) {
                    await this.loadTransactions();
                }
            } catch (error) {
                console.error('Error loading user data:', error);
            }
        },
        
        async loadAccounts() {
            try {
                const response = await this.apiCall(`/accounts?username=${this.currentUser.username}`);
                
                if (response.accounts) {
                    this.accounts = response.accounts.map(account => ({
                        ...account,
                        balance: parseFloat(account.balance) || 0
                    }));
                } else {
                    this.accounts = [];
                }
                
                // Set first account as selected if none selected
                if (this.accounts.length > 0 && !this.selectedAccount) {
                    this.selectedAccount = this.accounts[0];
                }
            } catch (error) {
                console.error('Error loading accounts:', error);
                this.accounts = [];
            }
        },
        
        async loadTransactions() {
            try {
                if (!this.selectedAccount) return;
                
                const response = await this.apiCall(`/transactions?account_number=${this.selectedAccount.account_number}`);
                
                if (response.transactions) {
                    // Process transactions to ensure proper type display
                    this.transactions = response.transactions.map(transaction => ({
                        ...transaction,
                        amount: parseFloat(transaction.amount) || 0,
                        // Keep original type for API compatibility, add display properties
                        typeDisplay: this.getTransactionTypeDisplay(transaction.type)
                    }));
                } else {
                    this.transactions = [];
                }
            } catch (error) {
                console.error('Error loading transactions:', error);
                this.transactions = [];
            }
        },
        
        // Account management
        async createAccount() {
            try {
                const response = await this.apiCall('/accounts/create', {
                    method: 'POST',
                    body: JSON.stringify({
                        username: this.currentUser.username,
                        account_type: this.createAccountForm.account_type,
                        initial_balance: this.createAccountForm.initial_balance
                    })
                });
                
                if (response.success) {
                    this.showCreateAccountModal = false;
                    this.successMessage = `Account created successfully! Account Number: ${response.account_number}`;
                    
                    // Clear form
                    this.createAccountForm = { account_type: 'SAVINGS', initial_balance: 0 };
                    
                    // Reload accounts
                    await this.loadAccounts();
                    
                    setTimeout(() => this.successMessage = '', 5000);
                } else {
                    throw new Error(response.message || 'Account creation failed');
                }
                
            } catch (error) {
                console.error('Account creation failed:', error);
            }
        },
        
        // Transaction methods
        async deposit() {
            try {
                const response = await this.apiCall('/transactions/deposit', {
                    method: 'POST',
                    body: JSON.stringify({
                        account_number: this.selectedAccount.account_number,
                        amount: this.depositForm.amount,
                        description: 'Deposit'
                    })
                });
                
                if (response.success) {
                    this.showDepositModal = false;
                    this.successMessage = `Deposit successful! New balance: ${this.formatCurrency(response.new_balance)}`;
                    
                    // Clear form
                    this.depositForm = { amount: 0 };
                    
                    // Reload data
                    await this.loadUserData();
                    
                    setTimeout(() => this.successMessage = '', 3000);
                } else {
                    throw new Error(response.message || 'Deposit failed');
                }
                
            } catch (error) {
                console.error('Deposit failed:', error);
            }
        },
        
        async withdraw() {
            try {
                const response = await this.apiCall('/transactions/withdraw', {
                    method: 'POST',
                    body: JSON.stringify({
                        account_number: this.selectedAccount.account_number,
                        amount: this.withdrawForm.amount,
                        description: 'Withdrawal'
                    })
                });
                
                if (response.success) {
                    this.showWithdrawModal = false;
                    this.successMessage = `Withdrawal successful! New balance: ${this.formatCurrency(response.new_balance)}`;
                    
                    // Clear form
                    this.withdrawForm = { amount: 0 };
                    
                    // Reload data
                    await this.loadUserData();
                    
                    setTimeout(() => this.successMessage = '', 3000);
                } else {
                    throw new Error(response.message || 'Withdrawal failed');
                }
                
            } catch (error) {
                console.error('Withdrawal failed:', error);
            }
        },
        
        async transfer() {
            try {
                const response = await this.apiCall('/transactions/transfer', {
                    method: 'POST',
                    body: JSON.stringify({
                        from_account: this.transferForm.from_account,
                        to_account: this.transferForm.to_account,
                        amount: this.transferForm.amount,
                        description: this.transferForm.description || 'Transfer'
                    })
                });
                
                if (response.success) {
                    this.showTransferModal = false;
                    this.successMessage = 'Transfer successful!';
                    
                    // Clear form
                    this.transferForm = { from_account: '', to_account: '', amount: 0, description: '' };
                    
                    // Reload data
                    await this.loadUserData();
                    
                    setTimeout(() => this.successMessage = '', 3000);
                } else {
                    throw new Error(response.message || 'Transfer failed');
                }
                
            } catch (error) {
                console.error('Transfer failed:', error);
            }
        },
        
        // UI helper methods
        formatCurrency(amount) {
            return new Intl.NumberFormat('en-US', {
                style: 'currency',
                currency: 'USD'
            }).format(amount || 0);
        },
        
        formatDate(dateString) {
            if (!dateString) return 'N/A';
            return new Date(dateString).toLocaleDateString();
        },
        
        // Updated method to use the new transaction type display
        getTransactionTypeClass(type) {
            const display = this.getTransactionTypeDisplay(type);
            return display.class;
        },
        
        // Modal helpers
        openDepositModal(account) {
            this.selectedAccount = account;
            this.depositForm = { amount: 0 };
            this.showDepositModal = true;
        },
        
        openWithdrawModal(account) {
            this.selectedAccount = account;
            this.withdrawForm = { amount: 0 };
            this.showWithdrawModal = true;
        },
        
        openTransferModal() {
            this.transferForm = { 
                from_account: this.selectedAccount?.account_number || '',
                to_account: '', 
                amount: 0, 
                description: '' 
            };
            this.showTransferModal = true;
        },
        
        // Account selection
        selectAccount(account) {
            this.selectedAccount = account;
            this.loadTransactions();
        }
    }));
    
    console.log('Alpine.js data registered successfully');
});
