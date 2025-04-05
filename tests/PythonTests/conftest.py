import pytest

def pytest_addoption(parser):
    parser.addoption("--auth-ip", action="store", default="localhost", help="IP address of the authentication service")
    parser.addoption("--auth-port", action="store", default="8080", help="Port of the authentication service")
