<?PHP
/**
 * example that fetches a user object
 *
 * $Id: example_LeaveFeedback.php,v 1.1 2004/10/28 17:14:53 schst Exp $
 *
 * @package     Services_Ebay
 * @subpackage  Examples
 * @author      Stephan Schmidt
 */
error_reporting(E_ALL);
require_once '../Ebay.php';
require_once 'config.php';

$session = Services_Ebay::getSession($devId, $appId, $certId);

$session->setToken($token);

$ebay = new Services_Ebay($session);

/**
 * get the user information
 */
$user = $ebay->GetUser('loislane-74');

$result = $user->LeaveFeedback('4501333179', 'positive', 'Fast payment, thank you!');

echo	"<pre>";
print_r($result);
echo	"</pre>";
?>