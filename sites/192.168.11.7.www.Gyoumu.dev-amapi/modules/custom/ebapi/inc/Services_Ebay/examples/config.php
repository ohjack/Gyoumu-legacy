<?PHP
$devId  = 'ebfad531-903c-41e4-b1af-d16369d81e75';

$appId  = 'Maolung71-9e40-49c5-9a37-aa903b58a1b';

$certId = '6cd3588e-d011-4618-96c6-c251c47f99c7';

$username = 'TESTUSER_maolung';

$password = '2xaugadr';

$token = 'AgAAAA**AQAAAA**aAAAAA**nwiMTg**nY+sHZ2PrBmdj6wVnY+sEZ2PrA2dj6wFk4GhCJCGpQSdj6x9nY+seQ**ASkBAA**AAMAAA**h2S/tokAFDJfAa0TuSHEYcXxVS4zN2FlxDBfXegfjJQ28h4bp6C6jYGXpa6KXTq1xfaw7dOdZYy2XuLaVngFN11Lj3aHjep6HMnvEye3ncgka6xRAmqcSX4SEavH1mBWhae7INI5YeIWBD0aYPicoC+Z6U+PXyOC6s9AWvQkNOxn1xhWen7oWJx20uETOSBUw/Bt4Y1ZuWCuBx5jkIgGEWw0GKyK9wwvCYGVeM6tfeFRt92K2iWsYvYoiWznfzEgpmpCtVQfjchiYJ+Lr+fJFvdJesiREPJJ5r40PGZSZUgeYeAVESme0UVdAqdqFiHKcFMg27UpRCgyNhRbINfWxwX6QLqWQrIm7xVlIVSQY5B5gPLr5guTLMSGOIVgbq+yRMCVd32avQGu5q7T4lZ6wxM7S76qhR15S49bD2RSleG3TaS2fqplWA9oEKhA5g17X7wn+qkRdPkNDmtbL+ZYp2pOH6K8saiLhuKvXtx78knpW555x79T/miqw7cXtMtHdHFq+zjGOgSvTuaYgtaSsDpxJcCsIO/7Qq/twYfyKIEGskXznguFcIQQ9PzeL/dBBWoPCLe9D3mSd/5/J9vXxv+OXYI5lhzFyy2BGfULFHNWrA7W8dc28V0F62E4fMYgK51okpOACVkxfGy6vqDEq3ach6EL8KJOYspv4TLL/zdTODQ6cUxccGmez2+55disfmrnZjeSJrFyw41LIiBXcPv8HzgFGYnJ6Z11Yqf59Ai4ImewhyNEN/1YqJLpbgH+';

if ($devId === '') {
    if (file_exists('config-local.php')) {
    	require_once 'config-local.php';
    }
}
if ($devId === '') {
    
    echo 'In order to use Services_Ebay, you must specify devId, appId, certId and a token.<br />';
    echo 'Please register at <a href="http://developer.ebay.com">http://developer.ebay.com</a> to get this IDs and then modify examples/config.php<br />';
    exit();
}
?>