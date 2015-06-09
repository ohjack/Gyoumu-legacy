<?PHP
/**
 * example that fetches all transactions for an item
 *
 * $Id: example_GetItemTransactions.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$transactions = $ebay->GetItemTransactions('4501333179', '2004-08-24 00:00:00', '2004-08-30 00:00:00');

echo 'Transactions overview:<br />';
echo '<pre>';
print_r($transactions->toArray());
echo '</pre>';

echo '<br />Iterate all transactions:<br />';

foreach ($transactions as $transaction) {
    echo '<pre>';
    print_r($transaction->toArray());
    echo '</pre>';
}

echo '<b>Use new detail level</b><br />';

$session->setDetailLevel(Services_Ebay::TRANSACTION_DETAIL_BUYER + Services_Ebay::TRANSACTION_DETAIL_SELLER);

$transactions = $ebay->GetItemTransactions('4501333179', '2004-08-24 00:00:00', '2004-08-30 00:00:00');

echo 'Transactions overview:<br />';
echo '<pre>';
print_r($transactions->toArray());
echo '</pre>';

echo '<br />Iterate all transactions:<br />';

foreach ($transactions as $transaction) {
    echo '<pre>';
    print_r($transaction->toArray());
    echo '</pre>';
}

?>