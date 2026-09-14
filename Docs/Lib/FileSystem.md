FileSystem module provides all the necessary functions for interacting with the host system's files & directories.

# Methods

### (BOOL) file_exists (STR path)
Checks if the *file* at `path` exists. If the location exists, but is not a file, will still return `false`.

```python
file_exists:'file.txt'; # true or false.
```

### (BOOL) dir_exists (STR path)
Checks if the *directory* at `path` exists. If the location exists, but is not a directory, will still return `false`.

```python
dir_exists:'directory/'; # true or false.
```

### (STR) read (STR path)
Read a file at `path`. Returns the raw byte string or an empty string if read failed.

```python
read:'file.txt';
```

### (BOOL) write (STR path, STR bytes)
Write `bytes` to a file at `path`. Returns `true` if succeeded.

```python
write:['file.txt', 'Hello World!'];
read:'file.txt'; # Hello World!
```

### (BOOL) make_dir (STR path)
Create a directory at `path`.

```python
const STR path = 'directory/';
dir_exists:path; # false.
make_dir:path; # Make directory.
dir_exists:path; # true.
```

### (ARR) paths_in_dir (STR path)
Returns all paths directly under the directory at `path`. Paths ending with "/" are directories, otherwise they are a file.

```python
import IO;

const ARR paths = paths_in_dir:'directory/';
for path in paths;
	IO.print:path;
/;
# Example outputs:
# file.txt
# file2.txt
# subdir/
```

### (BOOL) remove (STR path)
Permanently delete the file or directory at `path`. Directories can only be deleted if it contains no items. Returns `true` if succeeded.

```python
const STR path = 'file.txt';
file_exists:path; # true.
remove:path; # Delete file/dir.
file_exists:path; # false.
```

### (BOOL) move (STR path, STR new_path)
Move the item at `path` to `new_path`.
