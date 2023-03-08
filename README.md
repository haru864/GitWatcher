# Name

GitWatcher

search for Git Repositories that have local repository not updating remotes 

<!-- # DEMO

"hoge"の魅力が直感的に伝えわるデモ動画や図解を載せる -->

# Features

When you migrate your development environment, for example by replacing your PC,\
you can easily find Git local repositories with changes that have not been pushed to a remotes.

# Requirement

I have confirmed that "GitWatcher" works in the following environments.\
It will work on other Linux distributions with similar Linux kernels.

```
Distributor ID:	Ubuntu
Description:	Ubuntu 22.04.2 LTS
Release:		22.04
Codename:		jammy
Linux kernel:	5.19.0-35-generic
```

# Installation

Please download the following file and execute it in your local environment.

```
build/GitWatcher
```

# Usage

Set the directory as the first argument of the command.\
The command recursively searches under the set directory, detects Git local repositories\
that satisfy any of the following conditions, and outputs a list.

* Remote repository is not registered
* There is a commit history that has not been pushed to the remote repository
* Uncommitted modifications are in the local repository

```bash
GitWatcher <directory path>
```

# Note

Note that "GitWatcher" can detect the difference between the remote and\
the branch where HEAD is located, but not other branches.\
This feature will be implemented at a later date.

<!-- # Author

作成情報を列挙する

* 作成者
* 所属
* E-mail

# License
ライセンスを明示する

"hoge" is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).

社内向けなら社外秘であることを明示してる

"hoge" is Confidential. -->