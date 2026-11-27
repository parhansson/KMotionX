using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using KFlopWebNC.DOM_Helpers;

public static class DOMExtensions
{
    public static string GetElementHTML(this WebBrowser browser, string elementID)
    {
        var result = "";

        var element = browser.Document.GetElementById(elementID);
        if (element != null)
        {
            result = element.InnerHtml;
        }

        return result;
    }
    public static void SetElementHTML(this WebBrowser browser, string elementID, object text)
    {
        var element = browser.Document.GetElementById(elementID);
        if (element != null && text != null)
        {
            element.InnerHtml = text.ToString();
        }
    }

    public static string GetElementOuterHTML(this WebBrowser browser, string elementID)
    {
        var result = "";

        var element = browser.Document.GetElementById(elementID);
        if (element != null)
        {
            result = element.OuterHtml;
        }

        return result;
    }
    public static void SetElementOuterHTML(this WebBrowser browser, string elementID, object text)
    {
        var element = browser.Document.GetElementById(elementID);
        if (element != null && text != null)
        {
            element.OuterHtml = text.ToString();
        }
    }

    public static string GetElementText(this WebBrowser browser, string elementID)
    {
        var result = "";

        var element = browser.Document.GetElementById(elementID);
        if (element != null)
        {
            result = element.InnerText;
        }

        return result;
    }
    public static void SetElementText(this WebBrowser browser, string elementID, object text)
    { 
        var element = browser.Document.GetElementById(elementID);
        if (element != null && text !=null)
        {
            element.InnerText = text.ToString();
        } 
    }



    public static string GetAttribute(this WebBrowser browser, string elementID, string attribute)
    {
        var result = "";

        var element = browser.Document.GetElementById(elementID);
        if (element != null)
        {
            result = element.GetAttribute(attribute);
        }

        return result;
    }
    public static void SetAttribute(this WebBrowser browser, string elementID, string attribute, object text)
    {
        var element = browser.Document.GetElementById(elementID);
        if (element != null)
        {
            element.SetAttribute(attribute, text.ToString());
        }
    }

}
