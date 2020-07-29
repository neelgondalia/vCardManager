# vCard Manager


## Installation of Node App

### 1. Install

```Bash
# From the root of the directory
npm install
```

### 2. Running Server

```Bash
# PORT is your personally given port number, e.g. 1234
npm run dev PORT
# Server will be accessible at http://localhost:PORT
```

## Directory Structure

```Bash
# This contains the Backend Node Server, with our Web Application and API
app.js

# These are the package configuration files for npm to install dependencies
package.json
package-lock.json

# This is the Frontend HTML file that you see when you visit the document root
public/index.html

# This is the Frontend browser JavaScript file
public/index.js

# This is the Frontend Custom Style Sheet file
public/style.css

# This is the directory for uploaded .vcf files
upload/

# This is the directory where you put all your C parser code
parser/
```
You will need to add functionality to app.js, index.html, index.js and, if you wish, style.css.


## Package.json and Lock file

* You don't need to touch these files or add any external libraries, I've added the only ones you need
* Most popular programming languages have some package management system, PHP has Composer, Ruby has RubyGems, Python has Pip...
* Package manager allows us to install libraries by simply typing `npm install PACKAGE_NAME --save` and then `npm install` to fetch the latest version
* The packages are installed to `node_modules/` due to the essence of a package manager, we don't need to include `node_modules/` in source control, because anyone can type `npm install`

## How does everything work together?

1. Install the dependencies (only need to do this once) and spin up our node server
    * Note: We're using "nodemon" (instead of say `node run dev`) because it hot-reloads app.js whenever it's changed

2. View at http://localhost:PORT

3. The HTML is loaded when you visit the page and see forms, tables, content

4. The CSS is also loaded, and you'll see the page has style 

5. The JavaScript file is loaded (index.js) and will run a bunch of "on load" AJAX calls to populate dropdowns, change elements

6. When buttons are clicked, more AJAX calls are made to the backend, that recieve a response update the HTML

7. An AJAX call is made from your browser, it makes an HTTP (GET, POST...) call to our web server

8. The app.js web server receives the request with the route, and request data (JSON, url parameters, files...)

9. Express looks for the route you defined, then runs the callback function you provided

10. The callback function (for this module) should just return a hard coded JSON response

11. The AJAX call gets a response back from the server (either a 200 OK or maybe an error like a 404 not found) and either calls the "success" callback function or the "fail" function. If the success is called, "data" contains the returned JSON, and we can use it to update elements, e.g.`$('#elementId').html('<div>' + data["somekey"] + '</div>');` where there is a "div" somewhere with the "id" "elementId".
