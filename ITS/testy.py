# -*- coding: utf-8 -*-
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import Select
from selenium.webdriver.support.ui import WebDriverWait
from selenium.common.exceptions import NoSuchElementException
from selenium.common.exceptions import NoAlertPresentException
from datetime import datetime
import unittest, time, re 

g_timeout = 15

class TcInit(unittest.TestCase):

    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.implicitly_wait(g_timeout)
        self.base_url = "https://ipa.demo1.freeipa.org/ipa/ui/"
        self.verificationErrors = []
        self.accept_next_alert = True

    def is_element_present(self, how, what):
        try: self.driver.find_element(by=how, value=what)
        except NoSuchElementException as e: return False
        return True
    
    def is_alert_present(self):
        try: self.driver.switch_to_alert()
        except NoAlertPresentException as e: return False
        return True
    
    def close_alert_and_get_its_text(self):
        try:
            alert = self.driver.switch_to_alert()
            alert_text = alert.text
            if self.accept_next_alert:
                alert.accept()
            else:
                alert.dismiss()
            return alert_text
        finally: self.accept_next_alert = True
    
    def tearDown(self):
        self.driver.quit()
        self.assertEqual([], self.verificationErrors)
    
    def on_no_such_element(self):
        now = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        self.driver.get_screenshot_as_file('screenshot-%s.png' % now)
        print "I've created screenshot-%s.png" % now
        self.on_no_such_element()

class TcSuccLogin(TcInit):
    
    def test_succ_login_admin(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("admin")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("Secret123")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "#container"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")

        try:
            driver.find_element_by_css_selector("span.loggedinas").click()
            driver.find_element_by_css_selector("li.dropdown,.open")
            driver.find_element_by_link_text("Logout").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
    def test_succ_login_manager(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("manager")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("Secret123")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "#container"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")

        try:
            driver.find_element_by_css_selector("span.loggedinas").click()
            driver.find_element_by_css_selector("li.dropdown,.open")
            driver.find_element_by_link_text("Logout").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
    
    def test_succ_login_employee(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("employee")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("Secret123")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "#container"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")

        try:
            driver.find_element_by_css_selector("span.loggedinas").click()
            driver.find_element_by_css_selector("li.dropdown,.open")
            driver.find_element_by_link_text("Logout").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
    
    def test_succ_login_helpdesk(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("helpdesk")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("Secret123")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "#container"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")

        try:
            driver.find_element_by_css_selector("span.loggedinas").click()
            driver.find_element_by_css_selector("li.dropdown,.open")
            driver.find_element_by_link_text("Logout").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")

class TcFailLogin(TcInit):
    
    def test_fail_login_admin(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("admin")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("password")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()

    
    def test_fail_login_manager(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("manager")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("password")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()
    
    def test_fail_login_employee(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("employee")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("password")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()
    
    def test_fail_login_helpdesk(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("helpdesk")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("password")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()
            ## TODO: Check that we havent logged in and got error msg

class TcFailLoginMsg(TcInit):
    
    def test_fail_login_msg(self):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys("helpdesk")
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys("password")
            driver.find_element_by_css_selector("button[name=\"login\"").click()
        except NoSuchElementException as e:
            self.on_no_such_element()
        ##TODO: sign get error message and search it for words


#### We won't test session timeout. Final evaluation of project would take too long!

class TcGoBackBtn(TcInit):
    
    def test_gobackbtn_logout(self):
        driver = self.driver
        driver.get(self.base_url)
        driver.back()
        ##TODO: sign in an catch containers
        self.fail()






if __name__ == "__main__":
    unittest.main()

