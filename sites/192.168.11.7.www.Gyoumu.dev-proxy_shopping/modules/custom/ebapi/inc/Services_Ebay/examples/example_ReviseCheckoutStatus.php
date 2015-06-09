<?PHP
/**
 * example that fetches all transactions for an item
 *
 * $Id: example_ReviseCheckoutStatus.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$status = $ebay->ReviseCheckoutStatus('4501333179', '0', 1, 1);

if ($status === true) {
	echo 'Checkout was updated';
} else {
	echo 'An error occured';
}
?>