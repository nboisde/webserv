<head>
        <link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico"/>
            <title>Post</title>
            <link rel="stylesheet" href="/css/style.css">
        </head>
        <body>
            <header>
                <?php require('header.php'); 
                ?>
            </header>
            
            <h1>Upload your file</h1>
            <div class="upload">
                <form action="/php/upldDone.php" method="post" enctype="multipart/form-data">
                    <input type="file" name="file1">
                    <!-- <input type="file" name="file2">
                    <input type="file" name="file3"> -->
                    <p><button class="styled" type="submit">Submit</button></p>
                </form>
            </div>

            <h1>Fill in your profile</h1>
            <div class="informations">
                <form action="/php/profile.php" method="post">    
                    <label class="infos" for="f_name">First name</label>
                    <input type="text" name="firstname" /><br>
                    <label class="infos" for="l_name">Last Name</label>
                    <input type="text" name="lastname" /><br>
                    <label class="infos" for="login">Login</label>
                    <input type="text" name="login" /><br>
                    <input type="submit" value="OK">
                </form>
            </div>
        </body>
        <footer>
            <?php include('footer.php');
            ?>
        </footer>