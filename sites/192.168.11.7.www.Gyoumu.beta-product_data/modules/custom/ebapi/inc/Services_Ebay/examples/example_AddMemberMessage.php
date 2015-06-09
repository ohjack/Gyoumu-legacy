<?PHP
/**
 * example that fetches an item
 *
 * $Id: example_AddMemberMessage.php,v 1.1 2004/10/28 17:14:53 schst Exp $
 *
 * @package     Services_Ebay
 * @subpackage  Examples
 * @author      Stephan Schmidt
 */
error_reporting(E_ALL);
require_once '../Ebay.php';
require_once 'config-loislane-74.php';

$session = Services_Ebay::getSession($devId, $appId, $certId);
$session->setToken($token);
$ebay = new Services_Ebay($session);

$session->debug = 2;

$result = $ebay->AddMemberMessage('superman-74', 3, 1, 'Just testing', 'This is only a test');

if ($result === true) {
    echo 'Messsage has been sent.<br />';
} else {
    echo 'An error occured while sending the message.';
}
?>