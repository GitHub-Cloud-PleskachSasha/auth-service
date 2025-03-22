import pytest
import requests
import pymysql

# BASE_URL = "https://localhost:8080"

def pytest_addoption(parser):
    parser.addoption("--auth-ip", action="store", default="localhost", help="IP address of the authentication service")
    parser.addoption("--auth-port", action="store", default="8080", help="Port of the authentication service")

@pytest.fixture
def BASE_URL(request):
    ip = request.config.getoption("--auth-ip")
    port = request.config.getoption("--auth-port")
    return f"https://{ip}:{port}"
    
@pytest.fixture(autouse=True)
def clean_db():
    connection = pymysql.connect(
        host="localhost",
        user="root",
        password="root",
        database="authservice",
        port=3307,
        cursorclass=pymysql.cursors.DictCursor
    )
    
    try:
        with connection.cursor() as cursor:
            cursor.execute("DELETE FROM person;")
        connection.commit()
    finally:
        connection.close()
# -----------------------------------------

def test_registration_success(BASE_URL):
    url = f"{BASE_URL}/register"
    response = requests.post(
        url,
        data={
            "email": "testuser1@example.com",
            "password": "TestPass123!"
        },
        verify=False
    )
    assert response.status_code == 201, f"Expected 201, received {response.status_code}"
# -----------------------------------------

def test_registration_duplicate(BASE_URL):
    url = f"{BASE_URL}/register"
    user = {
        "email": "testuser2@example.com",
        "password": "TestPass123!"
    }
    response1 = requests.post(
        url, 
        data=user, 
        verify=False
    )
    assert response1.status_code == 201, f"Expected 201, received {response1.status_code}"
    
    response2 = requests.post(url, data=user, verify=False)
    assert response2.status_code == 500, f"Expected 500 when duplicating, received{response2.status_code}"
# -----------------------------------------

def test_login_success(BASE_URL):
    register_url = f"{BASE_URL}/register"
    user = {
        "email": "testuser3@example.com",
        "password": "TestPass123!"
    }

    requests.post(register_url, data=user, verify=False)
    
    login_url = f"{BASE_URL}/login"
    response = requests.get(login_url, params=user, verify=False)
    assert response.status_code == 200, f"Expected 200, received {response.status_code}"
    
    data = response.json()
    assert "token" in data, "The response does not contain a JWT token"
# -----------------------------------------

def test_login_invalid(BASE_URL):
    login_url = f"{BASE_URL}/login"
    invalid_user = {"email": "nonexistent@example.com", "password": "WrongPassword"}
    response = requests.get(login_url, params=invalid_user, verify=False)
    assert response.status_code == 401, f"Expected 401, got it {response.status_code}"
# -----------------------------------------

@pytest.fixture
def jwt_token(BASE_URL):
    register_url = f"{BASE_URL}/register"
    user = {
        "email": "testuser4@example.com",
        "password": "TestPass123!"
    }
    requests.post(register_url, data=user, verify=False)
    
    login_url = f"{BASE_URL}/login"
    response = requests.get(login_url, params=user, verify=False)
    data = response.json()
    token = data.get("token")
    assert token is not None, "JWT token not received during login"
    return token

def test_validate_success(jwt_token, BASE_URL):
    url = f"{BASE_URL}/validate"
    payload = {
        "token": jwt_token
    }
    response = requests.get(url, params=payload, verify=False)

    assert response.status_code == 200, f"Expected 200, received {response.status_code}"

def test_validate_invalid_token(BASE_URL):
    url = f"{BASE_URL}/validate"
    payload = {
        "token": "invalidtoken"
    }
    response = requests.get(url, params=payload, verify=False)
    assert response.status_code == 401, f"Expected 401 or 403, received{response.status_code}"
    
