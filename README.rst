###################################################
Cycamore : The CYClus Additional MOdules REpository
###################################################

Additional modules for the Cyclus nuclear fuel cycle simulator from the 
University of Wisconsin - Madison are intended to be support innovative 
fuel cycle simulations with the Cyclus fuel cycle simulator. 

To see user and developer documentation for this code, please visit 
the `Cycamore Homepage`_.

********************************
Building and Installing Cycamore
********************************

In order to facilitate future compatibility with multiple platforms, 
Cycamore is built using `CMake`_. A full list of the Cycamore package 
dependencies is show below:

====================   ==================
Package                Minimum Version   
====================   ==================
`CMake`                2.8            
`boost`                1.34.1
`libxml2`              2                 
`Cyclus`               0.1            
====================   ==================

As with all software, the build/install can be broken into two steps:

  #. building and installing the dependencies
  #. building and installing Cycamore

Installing Dependencies
=======================

This guide assumes that the user has root access (to issue sudo 
commands) and access to a package manager or has some other suitable 
method of automatically installing established libraries. This process
was tested using a fresh install of Ubuntu 12.10. 

Cyclus
------

The Cyclus Library is the simulation-engine core of the more general Cyclus
package, of which Cycamore is a part. Detailed build/install instructions are
provided in the `Cyclus readme`_. We assume that the user has successfully built
and installed Cyclus (as well as CMake) for the following discussion.

All Others
----------

All other dependencies are common libraries available through package
managers. We provide an example using `apt-get`_. All required 
commands will take the form of:

.. code-block:: bash

  sudo apt-get install package

Where you will replace "package" with the correct package name. The
list of required package names are:

  #. libboost-all-dev
  #. libxml++2.6-dev

So, for example, in order to install libxml++ on your system, you will
type:

.. code-block:: bash

  sudo apt-get install libxml++2.6-dev

Let us take a moment to note the Boost library dependency. As it 
currently stands, we in fact depend on a small subset of the Boost 
libraries:

  #. libboost-filesystem-dev
  #. libboost-system-dev

However, it is possible (likely) that additional Boost libraries will
be used because they are an industry standard. Accordingly, we suggest
simply installing libboost-all-dev to limit any headaches due to 
possible dependency additions in the future.

Installing Cycamore
===================

Assuming you have the dependencies installed correctly, it's pretty
straightforward to install Cycamore. We make the following assumptions
in this guide:

  #. there is some master directory in which you're placing all
     Cyclus-related files called .../cyclus
  #. you have a directory named .../cyclus/install in which you plan
     to install all Cyclus-related files
  #. you have acquired the Cycamore source code from the 
     `Cycamore repo`_
  #. you have placed the Cycamore repository in .../cyclus/cycamore
  #. you have installed Cyclus  in .../cyclus/install (see the `Cyclus readme`_)

Under these assumptions **and** if you used a package manager to 
install coin-Cbc (i.e. it's installed in a standard location), the
Cyclus building and installation process will look like:

.. code-block:: bash

    .../cyclus/cycamore$ python install.py --prefix=../install --cyclus_root=../install

If you have installed coin-Cbc from source or otherwise have it 
installed in a non-standard location, you should make use of the
``coinRoot`` flag. The otherwise identical process would look 
like:

.. code-block:: bash

    .../cyclus/cycamore$ python install.py --prefix=../install --coin_root=/path/to/coin --cyclus_root=../install

Additionally, if you have installed Boost from source or otherwise have it
installed in a non-standard location, you should make use of the
``boostRoot`` flag. The otherwise identical process would look
like:

.. code-block:: bash

    .../cyclus/cycamore$ python install.py --prefix=../install --coin_root=/path/to/coin --cyclus_root=../install --boost_root=/path/to/boost


.. _`CMake`: http://www.cmake.org
.. _`apt-get`: http://linux.die.net/man/8/apt-get
.. _`Cyclus Homepage`: http://cyclus.github.com
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cyclus readme`: http://github.com/cyclus/cyclus
.. _`Cycamore Homepage`: http://cycamore.github.com
.. _`Cycamore repo`: https://github.com/cyclus/cycamore
.. _`Cycamore readme`: https://github.com/cyclus/cycamore

**********************
The Developer Workflow
**********************

*Note that "upstream" repository refers to the primary `cyclus/cycamore` repository.*

As you do your development, push primarily only to your own fork. Push to
the upstream repository (usually the "develop" branch) only after:

  * You have pulled the latest changes from the upstream repository.
  * You have completed a logical set of changes.
  * Cyclus compiles with no errors.
  * All tests pass.
  * Cyclus input files run as expected.
  * (recommended) your code has been reviewed by another developer.

Code from the "develop" branch generally must pass even more rigorous checks
before being integrated into the "master" branch. Hotfixes would be a
possible exception to this.

Workflow Notes
==============

  * Use a branching workflow similar to the one described at
    http://progit.org/book/ch3-4.html.

  * The "develop" branch is how cycamore developers will share (generally compilable) progress
    when we are not yet ready for the code to become 'production'.

  * Keep your own "master" and "develop" branches in sync with the upstream repository's
    "master" and "develop" branches. The master branch should always be the 'stable'
    or 'production' release of cyclus.
    
     - Pull the most recent history from the upstream repository "master"
       and/or "develop" branches before you merge changes into your
       corresponding local branch. Consider doing a rebase pull instead of
       a regular pull or 'fetch and merge'.  For example::

         git checkout develop
         git pull --rebase upstream develop

     - Only merge changes into your "master" or "develop" branch when you
       are ready for those changes to be integrated into the upstream
       repository's corresponding branch. 

  * As you do development on topic branches in your own fork, consider rebasing
    the topic branch onto the "master" and/or "develop"  branches after *pulls* from the upstream
    repository rather than merging the pulled changes into your branch.  This
    will help maintain a more linear (and clean) history.
    *Please see caution about rebasing below*.  For example::

      git checkout [your topic branch]
      git rebase develop

  * **Passing Tests**

      - To check that your branch passes the tests, you must build and install your topic 
        branch and then run the CycamoreUnitTestDriver (at the moment, ```make 
        test``` is insufficient). For example ::
      
          mkdir install
          python install.py --prefix=../install ...
          ../install/cycamore/bin/CycamoreUnitTestDriver

      - There are also a suite of sample input files 
        In addition to the CycamoreUnitTestDriver, a suite of input files can be run and 
        tested using the run_inputs.py script that is configured, built, and installed 
        with Cycamore. It relies on the input files that are part of your Cycamore 
        repository, and only succeeds for input files that are correct (some may have 
        known issues. See the issue list in cycamore for details.) To run the example 
        input files, ::

          python ../install/cycamore/bin/run_inputs.py

  * **Making a Pull Request** 
    
      - When you are ready to move changes from one of your topic branches into the 
        "develop" branch, it must be reviewed and accepted by another 
        developer. 

      - You may want to review this `tutorial <https://help.github.com/articles/using-pull-requests/>`_ 
        before you make a pull request to the develop branch.
        
  * **Reviewing a Pull Request** 

     - Build, install, and test it. If you have added the remmote repository as 
       a remote you can check it out and merge it with the current develop 
       branch thusly, ::
       
         git checkout -b remote_name/branch_name
         git merge develop

     - Look over the code. 

        - Check that it meets `our style guidelines <http://cyclus.github.com/devdoc/style_guide.html>`_.

        - Make inline review comments concerning improvements. 
      
     - Accept the Pull Request    

        - In general, **every commit** (notice this is not 'every push') to the
          "develop" and "master" branches should compile and pass tests. This
          is guaranteed by using a NON-fast-forward merge during the pull request 
          acceptance process. 
    
        - The green "Merge Pull Request" button does a non-fast-forward merge by 
          default. However, if that button is unavailable, you've made minor 
          local changes to the pulled branch, or you just want to do it from the 
          command line, make sure your merge is a non-fast-forward merge. For example::
          
            git checkout develop
            git merge --no-ff remote_name/branch_name -m "A message""

Cautions
========

  * **NEVER** merge the "master" branch into the "develop"
    branch. Changes should only flow *to* the "master" branch *from* the
    "develop" branch.

  * **DO NOT** rebase any commits that have been pulled/pushed anywhere
    else other than your own fork (especially if those commits have been
    integrated into the upstream repository.  You should NEVER rebase
    commits that are a part of the 'master' branch.  *If you do, you will be
    flogged publicly*.

  * Make sure that you are pushing/pulling from/to the right branches.
    When in doubt, use the following syntax::

      git push [remote] [from-branch]:[to-branch]

    and (*note that pull always merges into the current checked out branch*)::

      git pull [remote] [from-branch]

An Example
==========

Introduction
------------

As this type of workflow can be complicated to converts from SVN and very complicated
for brand new programmers, an example is provided.

For the sake of simplicity, let us assume that we want a single "sandbox" branch
in which we would like to work, i.e. where we can store all of our work that may not
yet pass tests or even compile, but where we also want to save our progress. Let us 
call this branch "Work". So, when all is said and done, in our fork there will be 
three branches: "Master", "Develop", and "Work".


Acquiring Cycamore and Workflow
-------------------------------

We begin with a fork of the main ("upstream") Cycamore repository. After initially forking
the repo, we will have two branches in our fork: "Master" and "Develop".

Acquiring a Fork of the Cycamore Repository
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A fork is *your* copy of Cycamore. Github offers an excelent 
`tutorial <http://help.github.com/fork-a-repo/>`_ on how to set one up. The rest of this
example assumes you have set up the "upstream" repository as cyclus/cycamore. Note that git
refers to your fork as "origin".

First, let's make our "work" branch:
::

    .../cycamore_dir/$ git branch work
    .../cycamore_dir/$ git push origin work


We now have the following situation: there exists the "upstream" copy of the Master and
Develop branches, there exists your fork's copy of the Master, Develop, and Work branches,
*AND* there exists your *local* copy of the Master, Develop, and Work branches. It is 
important now to note that you may wish to work from home or the office. If you keep your 
fork's branches up to date (i.e., "push" your changes before you leave), only your *local*
copies of your branches may be different when you next sit down at the other location.

Workflow: The Beginning
^^^^^^^^^^^^^^^^^^^^^^^

Now, for the workflow! This is by no means the only way to perform this type of workflow, 
but I assume that you wish to handle conflicts as often as possible (so as to keep their total 
number small). Let us imagine that you have been at work, finished, and successfully pushed 
your changes to your *Origin* repository. You are now at home, perhaps after dinner (let's just 
say some time has passed), and want to continue working a bit (you're industrious, I suppose... 
or a grad student). To begin, let's update our *home's local branches*.
::

    .../cycamore_dir/$ git checkout develop
    .../cycamore_dir/$ git pull origin develop 
    .../cycamore_dir/$ git pull upstream develop
    .../cycamore_dir/$ git push origin develop

    .../cycamore_dir/$ git checkout work
    .../cycamore_dir/$ git pull origin work
    .../cycamore_dir/$ git merge develop
    .../cycamore_dir/$ git push origin work

Perhaps a little explanation is required. We first want to make sure that this new local copy of 
the develop branch is up-to-date with respect to the remote origin's branch and remote upstream's
branch. If there was a change from the remote upstream's branch, we want to push that to origin. 
We then follow the same process to update the work branch, except:

#. we don't need to worry about the *upstream* repo because it doesn't have a work branch, and
#. we want to incorporate any changes which may have been introduced in the develop branch update.

Workflow: The End
^^^^^^^^^^^^^^^^^

As time passes, you make some changes to files, and you commit those changes (to your *local work
branch*). Eventually (hopefully) you come to a stopping point where you have finished your project 
on your work branch *AND* it compiles *AND* it runs input files correctly *AND* it passes all tests!
Perhaps you have found Nirvana. In any case, you've performed the final commit to your work branch,
so it's time to make a pull request online and wait for our developer friends to 
review and accept it.

Sometimes, your pull request will be closed by the reviewer until further 
changes are made to appease the reviewer's concerns. This may be frustrating, 
but please act rationally, discuss the issues on the github space made for your 
pull request, consult the `style guide <http://cyclus.github.com/devdoc/style_guide.html>`_, 
email the developer listhost for further advice, and make changes to your topic branch 
accordingly. The pull request will be updated with those changes when you push them 
to your fork.  When you think your request is ready for another review, you can 
reopen the review yourself with the button made available to you. 

See also
--------

A good description of a git workflow with good graphics is available at
http://nvie.com/posts/a-successful-git-branching-model/
