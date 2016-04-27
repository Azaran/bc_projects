# Master Test Plan

### Test Plan Identifier:

freeIPA-loginpage-xvecer18-M-1

### Introduction:
#### Objectives:
Purpose of tests described by this document is to verify functionality of login mechanism and of proper response from the system. We aim to ensure correct behaviour of the first steps need for successful work with the system. 

#### Constraints:
During testing we will be limited by the testing tool [Selenium](http://www.seleniumhq.org/). In the meaning that the tool is not fully or not at all supporting some technologies which are used on the Web. Other thing is that Selenium doesn't have to work the same way in different web browsers.

### Test Items:
Purpose of this Master Test Plan is to test login and post-login pages web UI of project freeIPA in the version 4.3.0. Login and the root page of our testing can be found [here](https://ipa.demo1.freeipa.org/ipa/ui/).

This means that we have to test:
* Form entries for various inputs.
* Error warnings upon login (wrong credentials, empty field etc.).
* Default page of the system upon logging into it.
* Verifying whether user received interface matching his role in the system

### Features to be Tested:
We have to test following functionalities:
1. Login entries and all responses of login script.
2. Whether user received proper UI after successful login into the system.
  * Every role has different UI. In out system it means that we have to test 4 different user types and 2 user groups.

### Features Not to Be Tested:
All the features of the system which aren't marked as "to be Tested". The reason is fact that this is school project and tests for other features are covered in other tasks.

### Approach:
Based on the task given this test plan covers automated integration black-box testing of web GUI. 
As a tool for making automation tests will be used already mentioned Selenium with tests saved as C# code and web browser Mozzila Firefox as environment where we'll use Selenium.

### Item Pass/Fail Criteria:
All test cases has to be completed.

### Suspension Criteria and Resumption Requirements:
Testing can be suspended only between single test cases. Resumption is requires that state of the system nor the session is the same as when we suspended the testing.

### Test Deliverables:
1. This document as it is.
2. Test Cases Overview (section Appendix 1)
3. Test Scripts (in C#)
4. Test Reports
5. Test Log

### Test Environment:
1. Mozzila Firefox.
2. Selenium.
3. Any operation system.
4. Server with demonstration application (https://ipa.demo1.freeipa.org/ipa/ui/).
5. There has to be 4 user accounts representing each role in the system (admin, helpdesk, manager, employee) with appropriate rights for the role.

### Schedule:
Testing will take place untill June, 2016. Reason for this short period of time is that we will do the testing only during the time when course project will be active.

### Staffing and Training Needs:
Tests will be ran by myself only for stated reasons.
As for the skills I have to learn how to work with the mentioned tools in a way required by the testing tasks.

### Assumptions and Dependencies:
  * List the assumptions that have been made during the preparation of this plan.
  * List the dependencies.

### Approvals:
This test plan has to be approved by Ing. Aleš Smrčka, Ph.D. and people from Red Hat providing us the opportunity to use their application for this project.


# Appendix 1 - Test Case Overview
## Prerequisites of test cases:
* Our root URL of the application is https://ipa.demo1.freeipa.org/ipa/ui/#

## Test Cases:
### Login test cases:
#### General:
[comment]: <> (Verify that the login screen is having option to enter username and password with submit button)
* ** Login form is present: **
	1. Open application.
	2. Identify `<input>` elements with names `username` and `password`.
	3. Verify whether both are present and if their types are "text" and "password". If both are present, test has passed.

	
* ** We are able to login successfully: **
	1. Open application.
	2. Write one username from the list bellow and password into the form.
		- The usernames are:
			* `admin`,
			* `helpdesk`, 
			* `manager`, 
			* `employee`.
		- The password is `Secret123` for all 4 of them.
	3. Submit the form.
	4. Verify we have successfully logged into the system.
	5. Click on username in right top corner => in menu pick logout.
	6. If you haven't tested all default username move to step 2, else go to step 7.
	7. Verify that we have managed to successfully logged in with all usernames. If yes, test has passed but if at least one login wasn't successful test has failed.

	
* ** We are not able to login successfully: **
	1. Open application.
	2. Write one pair of username and password from the table bellow into the form.
		* Username   => Password 
		* `admin    => password`
		* `helpesk  => password`
		* `manager  => password`
		* `employee => password`
		* `doctor   => Secret123`
		* `doctor   => password`
	3. Submit the form.
	4. Verify we have successfully logged into the system.
	5. Click on username in right top corner => in menu pick logout.
	6. If you haven't tested all default usernames move to step 2, else go to step 7.
	7. Verify that we haven't managed to successfully logged in with all usernames. If we haven't, test has passed if at least one combination lead to successful login test has failed.

	
* ** Error message upon unsuccessful login contains "username or password": **
	1. Open application.
	2. Write username `admin` and password `test` into the form.
	3. Submit the form.
	4. Find `<div>` element with name `validation`.
	5. Verify the text inside the element contains sub-strings `username or password`. If yes, test has passed.

	
* ** Session timeout: **
	1. Open application.
	2. Log into the system as `admin` with password `Secret123`.
	3. Wait XY minutes.
	4. Refresh the web page.
	4. Find `<div>` element with name `validation`.
	5. Verify the text inside the element contains sub-strings `expired`. If yes, test has passed.

	
* ** "Go To Previous Page" doesn't cause logout: **
	1. Open application.
	2. Log into the system as `admin` with password `Secret123`.
	3. Click "Go To Previous Page" button in your browser (left arrow in top-left corner).
	4. Verify if you have got the same page as before clicking it. If yes, test has passed.

	
#### SQL injection:
* ** Blind injection test: **
	1. Open application.
	2. Write `sleep(30)` into both form entries.
	3. Submit the form.
	4. Verify if the time needed for verifying the entries was longer than 30 seconds. If yes, test has failed otherwise it passed.

	
* ** Error message test: **
	1. Open application.
	2. Write `'"` into both form entries.
	3. Submit the form.
	4. Verify if there is string `You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"'` somewhere on the site. If yes, test has failed, otherwise it passed.

	
* ** Basic tautology based SQL injection: **
	1. Open application.
	2. Write `' or 1=1 '` into both form entries.
	3. Submit the form.
	4. Verify that we haven't logged into the system. Look for `<div>` with name `validation` and check if the text contains sub-string `incorrect`.

	
#### XSS vulnerability:
[comment]: <> (Verify if XSS vulnerability work on login page)
* ** Basic script filtering: **
	1. Open application.
	2. Write `<script>alert(document.cookie);</script>` into both entries.
	3. Submit the form.
	4. Verify if whether some alert window popped up. If no, test has passed but if yes test has failed.

	
* ** Different char encoding for script brackets: **
	1. Open application.
	2. Write `%3cscript%3ealert(document.cookie);%3c/script%3e` into both entries.
	3. Submit the form.
	4. Verify whether some alert window popped up. If no, test has passed but if yes test has failed.

	
* ** Basic script filtering: **
	1. Open application.
	2. Write `\x3cscript\x3ealert(document.cookie);\x3c/script\x3e` into both entries.
	3. Submit the form.
	4. Verify whether some alert window popped up. If no, test has passed but if yes test has failed.

	
### User types:
* ** Administrator: **
	1. Open application.
	2. Login as `admin` with password `Secret123`.
	3. Open menu in top-right corner.
	4. Select item `Profile`.
	5. Find and click on button `User Groups`.
	6. Verify that there are `admins` and `trust admins` in the list of group. If yes, test passed otherwise failed.

	
* ** Helpdesk: **
	1. Open application.
	2. Login as `helpdesk` with password `Secret123`.
	3. Open menu in top-right corner.
	4. Select item `Profile`.
	5. Find and click on button `User Groups`.
	6. Verify that there are `admins` and `trust admins` in the list of group. If yes, test passed otherwise failed.

	
* ** Manager: **
	1. Open application.
	2. Login as `manager` with password `Secret123`.
	3. Open menu in top-right corner.
	4. Select item `Profile`.
	5. Find and click on button `User Groups`.
	6. Verify that there are `admins`, `managers`, `employees` and `ipausers` in the list of group. If yes, test passed otherwise failed.

	
* ** Employee: **
	1. Open application.
	2. Login as `employee` with password `Secret123`.
	3. Open menu in top-right corner.
	4. Select item `Profile`.
	5. Find and click on button `User Groups`.
	6. Verify that there are `admins`, `employees` and `ipausers` in the list of group. If yes, test passed otherwise failed.

	
### User interface:
* ** UI for administrator: **
	1. Open application.
	2. Login as `admin` with password `Secret123`.
	3. Verify that there are 3 rows with width of 100% of the screen containing menus and rest of the screen is the content area.
	4. Verify that the Main-menu contains items:
		* Identity,
		* Policy,
		* Authentication,
		* Network Services,
		* IPA Server.
	5. Verify that by default the Sub-menu after login contains:
		* Users,
		* User groups,
		* Hosts,
		* Host groups,
		* Netgroups,
		* Services,
		* Automember.
	6. Click on `Policy` in main menu.
	7. Verify that the Sub-menu contains:
		* Host Based Access Control,
		* Sudo,
		* SELinux User Maps,
		* Password Policies,
		* Kerberos Ticket Policy.
	8. Click on `Authentication` in main menu.
	9. Verify that the Sub-menu contains:
		* Certificates,
		* OTP Tokens,
		* RADIUS Servers.
	10. Click on `Network Services` in main menu.
	11. Verify that the Sub-menu contains:
		* Automount,
		* DNS.
	12. Click on `IPA Server` in main menu.
	13. Verify that the Sub-menu contains:
		* Role Based Access Control,
		* ID Ranges,
		* ID Views,
		* Realm Domains,
		* Trusts,
		* Topology,
		* API browser,
		* Configuration.
	14. If all verification were successful test has passed otherwise it has failed.

		
* **UI for helpdesk: **
	1. Open application.
	2. Login as `helpdesk` with password `Secret123`.
	3. Verify that there are 3 rows with width of 100% of the screen containing menus and rest of the screen is the content area.
	4. Verify that the Main-menu contains items:
		* Identity,
		* Policy,
		* Authentication,
		* Network Services,
		* IPA Server.
	5. Verify that by default the Sub-menu after login contains:
		* Users,
		* User groups,
		* Hosts,
		* Host groups,
		* Netgroups,
		* Services,
		* Automember.
	6. Click on `Policy` in main menu.
	7. Verify that the Sub-menu contains:
		* Host Based Access Control,
		* Sudo,
		* SELinux User Maps,
		* Password Policies,
		* Kerberos Ticket Policy.
	8. Click on `Authentication` in main menu.
	9. Verify that the Sub-menu contains:
		* Certificates,
		* OTP Tokens,
		* RADIUS Servers.
	10. Click on `Network Services` in main menu.
	11. Verify that the Sub-menu contains:
		* Automount,
		* DNS.
	12. Click on `IPA Server` in main menu.
	13. Verify that the Sub-menu contains:
		* Role Based Access Control,
		* ID Ranges,
		* ID Views,
		* Realm Domains,
		* Trusts,
		* Topology,
		* API browser,
		* Configuration.
	14. If all verification were successful test has passed otherwise it has failed.

	
* ** UI for manager: **
	1. Open application.
	2. Login as `manager` with password `Secret123`.
	3. Verify that there are 2 rows with width of 100% of the screen containing menus and rest of the screen is the content area.
	4. Verify that the Main-menu contains items:
		* Users,
		* OTP Tokens.
	5. Verify that by default the content area contains table of `Active users`.
	6. Click on `OTP Tokens` in main menu.
	7. Verify that by default the content contains table of `OTP Tokens`.
	8. If all verifications were successful test has passed otherwise it has failed.

	
* ** UI for employee: **
	1. Open application.
	2. Login as `employee` with password `Secret123`.
	3. Verify that there are 2 rows with width of 100% of the screen containing menus and rest of the screen is the content area.
	4. Verify that the Main-menu contains items:
		* Users,
		* OTP Tokens.
	5. Verify that by default the content area contains table of `Active users`.
	6. Click on `OTP Tokens` in main menu.
	7. Verify that by default the content contains table of `OTP Tokens`.
	8. If all verifications were successful test has passed otherwise it has failed.
	