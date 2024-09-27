# Prompt Customization

To customize the shell's prompt set the `$PROMPT` environment variable to the prompt you want to show

```bash
export PROMPT='{red+bold}{cwd}{0} $ '
```

## Language

`{<code>}` is being used to apply styling to the text and adding things like current working directory, user etc to the prompt

## Colors and styles

Colors and Styles can be compined with other styles (i.e. bold+green or bold+under)

### Reset 

- `0`

### Colors

- `black`
- `red`
- `green`
- `yellow`
- `blue`
- `purple`
- `cyan`
- `lgrey`
- `dgrey`

### Style

- `bold`
- `under`
- `italic`

## Variables

- `user`
- `cwd`
- More coming soon...

## Example

The default prompt of ksh is the following

```c
"[{bold+blue}{user}{0}] {bold+green}{cwd}{0} > "
```
