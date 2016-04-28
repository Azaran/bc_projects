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

class TcBase(unittest.TestCase):

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
    
    def make_screenshot(self):
        now = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        name = self.__class__.__name__
        self.driver.get_screenshot_as_file('screenshot-%s-%s.png' % (name, now))
        print "I've created screenshot-%s-%s.png" % (name, now)

    def fill_submit_login_form(self, user, passwd):
        driver = self.driver
        driver.get(self.base_url)
        for i in range(g_timeout):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
            except: pass
            time.sleep(1)
        else: 
            self.fail("time out")
            return False
        try:
            driver.find_element_by_css_selector("#username1").click()
            driver.find_element_by_css_selector("#username1").send_keys(user)
            driver.find_element_by_css_selector("#password2").click()
            driver.find_element_by_css_selector("#password2").send_keys(passwd)
            driver.find_element_by_css_selector("button[name=\"login\"]").click()
        except NoSuchElementException as e:
            self.make_screenshot()
            return False
        return True
        

class TcSuccLogin(TcBase):

    def succ_login(self, user, passwd):
        driver = self.driver
        if (self.fill_submit_login_form(user, passwd)):
            for i in range(g_timeout):
                try:
                    if self.is_element_present(By.CSS_SELECTOR, "span.loggedinas"): break
                except: pass
                time.sleep(1)
            else:
                self.make_screenshot()
                self.fail("time out")

            try:
                driver.find_element_by_css_selector("span.loggedinas").click()
                driver.find_element_by_css_selector("li.dropdown.open")
                driver.find_element_by_link_text("Logout").click()
            except NoSuchElementException as e:
                self.make_screenshot()

            for i in range(g_timeout):
                try:
                    if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")

    def test_succ_login_admin(self):
        self.succ_login("admin", "Secret123")

    def test_succ_login_manager(self):
        self.succ_login("manager", "Secret123")
   
    def test_succ_login_employee(self):
        self.succ_login("employee", "Secret123")
    
    def test_succ_login_helpdesk(self):
        self.succ_login("helpdesk", "Secret123")

class TcFailLogin(TcBase):

    def fail_login(self, user, passwd):
        driver = self.driver
        if (self.fill_submit_login_form(user, passwd)):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else:
                self.make_screenshot()
                self.fail("time out")
        else:
            self.fail("couldn't fill and submit login form")
    
    def test_fail_login_admin(self):
        self.fail_login("admin", "password")
    
    def test_fail_login_manager(self):
        self.fail_login("manager", "password")
    
    def test_fail_login_employee(self):
        self.fail_login("employee", "password")
    
    def test_fail_login_helpdesk(self):
        self.fail_login("helpdesk", "password")
    
    def test_fail_login_doctor1(self):
        self.fail_login("doctor", "Secret123")
    
    def test_fail_login_doctor2(self):
        self.fail_login("doctor", "password")

class TcFailLoginMsg(TcBase):
    
    def test_fail_login_msg(self):
        driver = self.driver
        if (self.fill_submit_login_form("admin", "password")):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            el_text = driver.find_element_by_css_selector("div.fade.in.alert.alert-danger")
            text = el_text.get_attribute("innerHTML")
            r = re.compile(ur"(password)|(\sor\s)|(username)")
            if len(re.findall(r, text)) != 3:
                self.make_screenshot()
                self.fail("login error message")
        else:
            self.fail("couldn't fill and submit login form")
                
#### We won't test session timeout. Final evaluation of project would take too long!

class TcGoBackBtn(TcBase):
    
    def test_gobackbtn_logout(self):
        driver = self.driver
        if (self.fill_submit_login_form("helpdesk", "Secret123")):
            for i in range(g_timeout):
                try:
                    if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
                except: pass
                time.sleep(1)
            else: 
                self.fail("time out")
            
            driver.back()
            
            for i in range(g_timeout):
                try:
                    if self.is_element_present(By.CSS_SELECTOR, "label[name=\"username\"]"): break
                except: pass
                time.sleep(1)
            else: 
                self.fail("time out")
        else:
            self.fail("couldn't fill and submit login form")

class TcSQLInjection(TcBase):
    
    def test_blind_injection(self):
        driver = self.driver
        if (self.fill_submit_login_form("sleep(30)", "sleep(30)")):
            for i in range(30):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            if i >= 30:
                self.fail("\"sleep(30)\" probably worked")
        else:
            self.fail("couldn't fill and submit login form")

    def test_error_msg(self):
        driver = self.driver
        if (self.fill_submit_login_form("\'\"", "\'\"")):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            page = driver.page_source
            text_found = re.search(r"You have an error in your SQL syntax; check the manual that \
            corresponds to your MySQL server version for the right syntax to use near \'\"\'",page)
            if text_found is not None:
                self.fail("SQL syntax error")
        else:
            self.fail("couldn't fill and submit login form")

    def test_tautology(self):
        driver = self.driver
        if (self.fill_submit_login_form("\' or 1=1\'", "\' or 1=1\'")):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            
            el_text = driver.find_element_by_css_selector("div.fade.in.alert.alert-danger")
            text = el_text.get_attribute("innerHTML")
            text_found = re.search(r"(incorrect)", text)
            if text_found is None:
                self.fail("logged in")
        else:
            self.fail("couldn't fill and submit login form")

class TcXSSVulner(TcBase):


    def test_standard_brackets(self):
        driver = self.driver
        if (self.fill_submit_login_form("<script>alert(document.cookie);</script>",\
            "<script>alert(document.cookie);</script>")):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            if self.is_alert_present():
                self.fail("script worked")
        else:
            self.fail("couldn't fill and submit login form")


    def test_encoded_brackets1(self):
        driver = self.driver
        if (self.fill_submit_login_form("\%3cscript\%3ealert(document.cookie);\%3c/script\%3e",\
            "\%3cscript\%3ealert(document.cookie);\%3c/script\%3e")):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            if self.is_alert_present():
                self.fail("script worked")
        else:
            self.fail("couldn't fill and submit login form")
    
    def test_encoded_brackets2(self):
        driver = self.driver
        if (self.fill_submit_login_form("\\3cscript\\3ealert(document.cookie);\\3c/script\\3e",\
            "\\3cscript\\3ealert(document.cookie);\\3c/script\\3e")):
            for i in range(g_timeout):
                try:
                    el = driver.find_element_by_css_selector("div[name=\"validation\"]")
                    display = el.value_of_css_property('display')
                    if (display == "block"): break
                except: pass
                time.sleep(1)
            else: self.fail("time out")
            if self.is_alert_present():
                self.fail("script worked")
        else:
            self.fail("couldn't fill and submit login form")


class TcUserType(TcBase):

    def test_user_type_admin(self):
    def test_user_type_manager(self):
    def test_user_type_employee(self):
    def test_user_type_helpdesk(self):

class TcUserInterface(TcBase):
    def test_ui_admin(self):
    def test_ui_manager(self):
    def test_ui_employee(self):
    def test_ui_helpdesk(self):



if __name__ == "__main__":
    unittest.main()

