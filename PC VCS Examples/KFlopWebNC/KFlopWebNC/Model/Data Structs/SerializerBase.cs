
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization.Formatters.Binary;
using System.Xml.Serialization;
using System.IO;
using System.Xml;
using System.Collections;


namespace KFlopWebNC.Model
{
    [Serializable()]
    public abstract class SerializeBase<T>
    {
        #region XML Serialization
        public static string ToXMLString(T tosave)
        {
            string xml = "";
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                using (StringWriter stringWriter = new StringWriter())
                {
                    serializer.Serialize(stringWriter, tosave);
                    xml = stringWriter.ToString();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error Creating XML String :: " + ex.Message.ToString());
            }
            return xml;
        }
        public static T FromXMLString(string data)
        {
            try
            {
                System.Xml.Serialization.XmlSerializer serializer =
                  new System.Xml.Serialization.XmlSerializer(typeof(T));

                using (StringReader reader = new StringReader(data))
                {
                    return (T)serializer.Deserialize(reader);
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message.ToString());
                throw new Exception("Failed to create object from xml string", ex);
            }
        }
        public static void SaveXML(string filename, T tosave)
        {
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                using (TextWriter textWriter = new StreamWriter(filename))
                {
                    serializer.Serialize(textWriter, tosave);
                    textWriter.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error Saving File :: " + ex.Message.ToString());
                throw new Exception("Error Saving File :: ", ex);
            }
        }
        public static T LoadXML(string filename)
        {
            if (System.IO.File.Exists(filename) == true)
            {
                try
                {
                    System.Xml.Serialization.XmlSerializer serializer =
                      new System.Xml.Serialization.XmlSerializer(typeof(T));

                    using (TextReader reader = new StreamReader(filename))
                    {
                        return (T)serializer.Deserialize(reader);
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message.ToString());
                    throw new InvalidOperationException("Failed to create object from xml string", ex);
                }
            }
            else
            {
                throw new System.IO.FileNotFoundException("Requested Data Source not found");
            }
        }

        public IEnumerable<K> ReadStream<K>(string strUrl, params string[] nodes) where K : SerializeBase<K>
        {
            List<K> retval = new List<K>();
            string uri = "";
            string name = "";
            Hashtable attributes = new Hashtable();
            try
            {
                using (var reader = XmlReader.Create(strUrl))
                {
                    while (reader.Read())
                    {
                        switch (reader.NodeType)
                        {
                            case XmlNodeType.Element:
                                attributes = new Hashtable();
                                uri = reader.NamespaceURI;
                                name = reader.Name;
                                if (reader.HasAttributes)
                                {
                                    for (int i = 0; i < reader.AttributeCount; i++)
                                    {
                                        reader.MoveToAttribute(i);
                                        attributes.Add(reader.Name, reader.Value);
                                    }
                                }
                                if (name == "PartInfo")
                                {
                                    Console.WriteLine(reader.ReadInnerXml());
                                    var doc = new System.Xml.XmlDocument();
                                    XmlNode node = doc.ReadNode(reader);
                                    doc.AppendChild(node);
                                }
                                //reader.Value
                                //StartElement(strURI, strName, strName, attributes);
                                break;
                            case XmlNodeType.EndElement:
                                name = reader.Name;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            catch (XmlException e)
            {
                Console.WriteLine("error occured: " + e.Message);
            }
            return retval;
        }
        #endregion

        #region BIN Serialization
        public static byte[] ToByteArray(T tosave)
        {
            try
            {
                BinaryFormatter bformatter = new BinaryFormatter();


                using (MemoryStream stream = new MemoryStream())
                {
                    bformatter.Serialize(stream, tosave);

                    return stream.ToArray();
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message.ToString());
                throw new InvalidOperationException("Failed to create byte array", ex);
            }
        }
        public static T FromByteArray(byte[] data)
        {
            try
            {
                BinaryFormatter bformatter = new BinaryFormatter();


                using (MemoryStream stream = new MemoryStream(data))
                {
                    T retval = (T)bformatter.Deserialize(stream);
                    return retval;
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message.ToString());
                throw new InvalidOperationException("Failed to create object from binary stream", ex);
            }
        }
        public static void SaveBIN(string filename, T tosave)
        {
            try
            {
                BinaryFormatter bformatter = new BinaryFormatter();

                XmlSerializer serializer = new XmlSerializer(typeof(T));
                using (Stream stream = File.Open(filename, FileMode.Create))
                {
                    bformatter.Serialize(stream, tosave);
                    stream.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error Saving File :: " + ex.Message.ToString());
            }
        }
        public static T LoadBIN(string filename)
        {
            if (System.IO.File.Exists(filename) == true)
            {
                try
                {
                    BinaryFormatter bformatter = new BinaryFormatter();


                    using (Stream stream = File.Open(filename, FileMode.Open))
                    {
                        T retval = (T)bformatter.Deserialize(stream);
                        stream.Close();
                        return retval;
                    }

                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message.ToString());
                    throw new InvalidOperationException("Failed to create object from binary stream", ex);
                }

            }
            else
            {
                throw new System.IO.FileNotFoundException("Requested Data Source not found");
            }
        }
        #endregion
    }
}
