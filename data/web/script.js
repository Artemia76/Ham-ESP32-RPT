'use strict';

let currentUserRole = '';

async function checkAuth() {
    try {
        const response = await fetch('/auth/status');
        const data = await response.json();
        if (!data.authenticated) {
            window.location.href = '/login.html';
            return;
        }
        currentUserRole = data.role;
        document.getElementById('userRole').innerText = `Connecté en tant que : ${currentUserRole}`;
        
        if (currentUserRole === 'Administrateur') {
            document.getElementById('tabAdmin').classList.remove('hidden');
        }
    } catch (err) {
        console.error('Auth check failed', err);
    }
}

function showTab(tab) {
    document.querySelectorAll('.nav-tab').forEach(t => t.classList.remove('active'));
    document.querySelectorAll('.container').forEach(c => c.classList.add('hidden'));
    
    if (tab === 'dashboard') {
        document.getElementById('tabDashboard').classList.add('active');
        document.getElementById('dashboardSection').classList.remove('hidden');
    } else if (tab === 'admin') {
        document.getElementById('tabAdmin').classList.add('active');
        document.getElementById('adminSection').classList.remove('hidden');
        loadUsers();
    }
}

async function logout() {
    await fetch('/auth/logout', { method: 'POST' });
    window.location.href = '/login.html';
}

// Dashboard Logic
function initDashboard() {
    const RepControl = document.getElementById('RepControl');
    const SqlControl = document.getElementById('SqlControl');
    const sqlValue = document.getElementById('sql_value');

    // Initial state
    fetch('/get', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'Config=All'
    })
    .then(r => r.json())
    .then(data => {
        RepControl.checked = data.state;
        SqlControl.value = data.squelch;
        sqlValue.innerText = data.squelch;
    });

    RepControl.onchange = () => {
        const formData = new URLSearchParams();
        formData.append('Rep', RepControl.checked);
        fetch('/set', {
            method: 'POST',
            body: formData
        });
    };

    SqlControl.oninput = () => {
        sqlValue.innerText = SqlControl.value;
    };

    SqlControl.onchange = () => {
        const formData = new URLSearchParams();
        formData.append('Sql', SqlControl.value);
        fetch('/set', {
            method: 'POST',
            body: formData
        });
    };

    // RSSI Polling
    setInterval(() => {
        if (document.getElementById('dashboardSection').classList.contains('hidden')) return;
        fetch('/get', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: 'RSSI=Signal'
        })
        .then(r => r.text())
        .then(val => {
            document.getElementById("RSSI").innerHTML = val;
        });
    }, 2000);
}

// Admin Logic
async function loadUsers() {
    const response = await fetch('/admin/users');
    if (response.ok) {
        const users = await response.json();
        const tbody = document.getElementById('userListBody');
        tbody.innerHTML = '';
        users.forEach(user => {
            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td>${user.username}</td>
                <td>${user.role}</td>
                <td>
                    ${user.username !== 'admin' ? `<button class="btn-danger" onclick="deleteUser('${user.username}')">Supprimer</button>` : ''}
                </td>
            `;
            tbody.appendChild(tr);
        });
    }
}

async function addUser() {
    const username = document.getElementById('newUsername').value;
    const password = document.getElementById('newPassword').value;
    const role = document.getElementById('newRole').value;

    if (!username || !password) return alert('Champs requis');

    const formData = new FormData();
    formData.append('username', username);
    formData.append('password', password);
    formData.append('role', role);

    const response = await fetch('/admin/users/add', {
        method: 'POST',
        body: formData
    });

    if (response.ok) {
        loadUsers();
        document.getElementById('newUsername').value = '';
        document.getElementById('newPassword').value = '';
    } else {
        alert('Erreur lors de l\'ajout');
    }
}

async function deleteUser(username) {
    if (!confirm(`Supprimer ${username} ?`)) return;
    const formData = new FormData();
    formData.append('username', username);
    const response = await fetch('/admin/users/delete', {
        method: 'POST',
        body: formData
    });
    if (response.ok) loadUsers();
}

async function exportConfig() {
    const response = await fetch('/admin/config/export');
    const data = await response.json();
    const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'users_config.json';
    a.click();
}

async function importConfig(input) {
    if (!input.files.length) return;
    const file = input.files[0];
    const text = await file.text();
    
    const formData = new FormData();
    formData.append('config', text);
    
    const response = await fetch('/admin/config/import', {
        method: 'POST',
        body: formData
    });
    
    if (response.ok) {
        alert('Configuration importée avec succès');
        loadUsers();
    } else {
        alert('Erreur lors de l\'importation');
    }
}

window.onload = async () => {
    await checkAuth();
    initDashboard();
};
