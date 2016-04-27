# -*- coding: utf-8 -*-
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import Select
from selenium.common.exceptions import NoSuchElementException
from selenium.common.exceptions import NoAlertPresentException
import unittest, time, re

class TcLoginAdmin(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.implicitly_wait(15)
        self.base_url = "https://ipa.demo1.freeipa.org/"
        self.verificationErrors = []
        self.accept_next_alert = True
    
    def test_tc_login_admin(self):
        driver = self.driver
        driver.get(self.base_url + "ipa/ui/")
        for i in range(15):
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
        except NoSuchElementException as e: self.fail()

        for i in range(15):
            try:
                if self.is_element_present(By.CSS_SELECTOR, "#container"): break
            except: pass
            time.sleep(1)
        else: self.fail("time out")
        try:
            driver.find_element_by_css_selector("span.loggedinas").click()
            context = driver.find_element_by_css_selector("i.fa-sign-out").value_of_css_property("context")
            print "context: " + str(context)
            driver.find_element_by_link_text("Logout").click()
        except NoSuchElementException as e: self.fail()
    
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

if __name__ == "__main__":
    unittest.main()

