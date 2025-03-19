import pytest
import requests
import pymysql

BASE_URL = "https://localhost:8080"

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
@pytest.fixture
def new_user_1():
    return {
        "email": "testuser1@example.com",
        "password": "TestPass123!"
    }

def test_registration_success(new_user_1):
    url = f"{BASE_URL}/register"
    response = requests.post(url, data=new_user_1, verify=False)
    assert response.status_code == 201, f"Expected 201, received {response.status_code}"
# -----------------------------------------

@pytest.fixture
def new_user_2():
    return {
        "email": "testuser2@example.com",
        "password": "TestPass123!"
    }

def test_registration_duplicate(new_user_2):
    url = f"{BASE_URL}/register"
    response1 = requests.post(url, data=new_user_2, verify=False)
    assert response1.status_code == 201, f"Expected 201, received {response1.status_code}"
    
    response2 = requests.post(url, data=new_user_2, verify=False)
    assert response2.status_code == 500, f"Expected 500 when duplicating, received{response2.status_code}"
# -----------------------------------------

@pytest.fixture
def new_user_3():
    return {
        "email": "testuser3@example.com",
        "password": "TestPass123!"
    }

def test_login_success(new_user_3):
    register_url = f"{BASE_URL}/register"
    requests.post(register_url, data=new_user_3, verify=False)
    
    login_url = f"{BASE_URL}/login"
    response = requests.get(login_url, params=new_user_3, verify=False)
    assert response.status_code == 200, f"Expected 200, received {response.status_code}"
    
    data = response.json()
    assert "token" in data, "The response does not contain a JWT token"
# -----------------------------------------

@pytest.fixture
def test_login_invalid():
    login_url = f"{BASE_URL}/login"
    invalid_user = {"email": "nonexistent@example.com", "password": "WrongPassword"}
    response = requests.get(login_url, params=invalid_user, verify=False)
    assert response.status_code == 401, f"Expected 401, got it {response.status_code}"
# -----------------------------------------

@pytest.fixture
def new_user_4():
    return {
        "email": "testuser4@example.com",
        "password": "TestPass123!"
    }
@pytest.fixture
def jwt_token(new_user_4):
    register_url = f"{BASE_URL}/register"
    requests.post(register_url, data=new_user_4, verify=False)
    
    login_url = f"{BASE_URL}/login"
    response = requests.get(login_url, params=new_user_4, verify=False)
    data = response.json()
    token = data.get("token")
    assert token is not None, "JWT token not received during login"
    return token

def test_validate_success(jwt_token):
    url = f"{BASE_URL}/validate"
    payload = {
        "token": jwt_token
    }
    response = requests.get(url, params=payload, verify=False)

    assert response.status_code == 200, f"Expected 200, received {response.status_code}"

def test_validate_invalid_token():
    url = f"{BASE_URL}/validate"
    payload = {
        "token": "invalidtoken"
    }
    response = requests.get(url, params=payload, verify=False)
    assert response.status_code == 401, f"Expected 401 or 403, received{response.status_code}"
    
