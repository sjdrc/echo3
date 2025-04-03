/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package au.net.emblem.echotemplates.thirdpersongame;

import java.awt.Component;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Reader;
import java.nio.charset.Charset;
import java.text.MessageFormat;
import java.util.Enumeration;
import java.util.LinkedHashSet;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import javax.swing.JComponent;
import javax.swing.event.ChangeListener;
import org.netbeans.api.project.ProjectManager;
import org.netbeans.api.templates.TemplateRegistration;
import org.netbeans.spi.project.ui.support.ProjectChooser;
import org.netbeans.spi.project.ui.templates.support.Templates;
import org.openide.WizardDescriptor;
import org.openide.filesystems.FileObject;
import org.openide.filesystems.FileUtil;
import org.openide.util.Exceptions;
import org.openide.util.NbBundle;
import org.openide.util.NbBundle.Messages;
import org.openide.xml.XMLUtil;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

// TODO define position attribute
@TemplateRegistration(folder = "Project/Native", category = "Echo C++ Projects", position=2, displayName = "Echo Third Person game", description = "EchoThirdPersonGameDescription.html", iconBase = "au/net/emblem/echotemplates/thirdpersongame/EchoThirdPersonGame.png", content = "ThirdPersonGame.zip")
public class EchoThirdPersonGameWizardIterator implements WizardDescriptor./*Progress*/InstantiatingIterator {

    private int index;
    private WizardDescriptor.Panel[] panels;
    private WizardDescriptor wiz;

    public EchoThirdPersonGameWizardIterator() {
    }

    public static EchoThirdPersonGameWizardIterator createIterator() {
        return new EchoThirdPersonGameWizardIterator();
    }

    private WizardDescriptor.Panel[] createPanels() {
        return new WizardDescriptor.Panel[]{
            new EchoThirdPersonGameWizardPanel(),};
    }

    private String[] createSteps() {
        return new String[]{
            NbBundle.getMessage(EchoThirdPersonGameWizardIterator.class, "LBL_CreateProjectStep")
        };
    }

    public Set/*<FileObject>*/ instantiate(/*ProgressHandle handle*/) throws IOException {
        Set<FileObject> resultSet = new LinkedHashSet<FileObject>();
        File dirF = FileUtil.normalizeFile((File) wiz.getProperty("projdir"));
        dirF.mkdirs();

        FileObject template = Templates.getTemplate(wiz);
        FileObject dir = FileUtil.toFileObject(dirF);
        unZipFile(template.getInputStream(), dir);

        // Always open top dir as a project:
        resultSet.add(dir);
        // Look for nested projects to open as well:
        Enumeration<? extends FileObject> e = dir.getFolders(true);
        while (e.hasMoreElements()) {
            FileObject subfolder = e.nextElement();
            if (ProjectManager.getDefault().isProject(subfolder)) {
                resultSet.add(subfolder);
            }
        }

        File parent = dirF.getParentFile();
        if (parent != null && parent.exists()) {
            ProjectChooser.setProjectsFolder(parent);
        }

        return resultSet;
    }

    public void initialize(WizardDescriptor wiz) {
        this.wiz = wiz;
        index = 0;
        panels = createPanels();
        // Make sure list of steps is accurate.
        String[] steps = createSteps();
        for (int i = 0; i < panels.length; i++) {
            Component c = panels[i].getComponent();
            if (steps[i] == null) {
                // Default step name to component name of panel.
                // Mainly useful for getting the name of the target
                // chooser to appear in the list of steps.
                steps[i] = c.getName();
            }
            if (c instanceof JComponent) { // assume Swing components
                JComponent jc = (JComponent) c;
                // Step #.
                // TODO if using org.openide.dialogs >= 7.8, can use WizardDescriptor.PROP_*:
                jc.putClientProperty("WizardPanel_contentSelectedIndex", new Integer(i));
                // Step name (actually the whole list for reference).
                jc.putClientProperty("WizardPanel_contentData", steps);
            }
        }
    }

    public void uninitialize(WizardDescriptor wiz) {
        this.wiz.putProperty("projdir", null);
        this.wiz.putProperty("name", null);
        this.wiz = null;
        panels = null;
    }

    public String name() {
        return MessageFormat.format("{0} of {1}",
                new Object[]{new Integer(index + 1), new Integer(panels.length)});
    }

    public boolean hasNext() {
        return index < panels.length - 1;
    }

    public boolean hasPrevious() {
        return index > 0;
    }

    public void nextPanel() {
        if (!hasNext()) {
            throw new NoSuchElementException();
        }
        index++;
    }

    public void previousPanel() {
        if (!hasPrevious()) {
            throw new NoSuchElementException();
        }
        index--;
    }

    public WizardDescriptor.Panel current() {
        return panels[index];
    }

    // If nothing unusual changes in the middle of the wizard, simply:
    public final void addChangeListener(ChangeListener l) {
    }

    public final void removeChangeListener(ChangeListener l) {
    }
    
    private static String getFileExtension(String name)
    {
        try {
            return name.substring(name.lastIndexOf("."));
        } catch (Exception e) {
            return "";
        }

    }    
    private static void unZipFile(InputStream source, FileObject projectRoot) throws IOException {
        try {
            
            //As each file is unzipped we need to do the following:
            // - Check the name minus extension. If the name is MyApplication, change the name to MakeSourceSafe(projectRoot.getName())
            // - After each file is copied, open the file and perform a find and replace on MyApplication to MakeSourceSafe(projectRoot.getName())
            
            //Turn "My Cool Project" into "MyCoolProject"
            String codeSafeProjectName = projectRoot.getName().replaceAll("\\s+","_");
            
            ZipInputStream str = new ZipInputStream(source);
            ZipEntry entry;
            
            while ((entry = str.getNextEntry()) != null) {
                if (entry.isDirectory()) {
                    FileUtil.createFolder(projectRoot, entry.getName());
                } else {
                    String newFileName=entry.getName();
                    if(newFileName.contains("MyProjectApplication"))
                    {
                        newFileName=newFileName.replace("MyProjectApplication",codeSafeProjectName+"Application");
                    }else
                    {
                        if(newFileName.contains("MyProject"))
                        {
                            newFileName=newFileName.replace("MyProject",codeSafeProjectName);
                        }
                    }
                    FileObject fo = FileUtil.createData(projectRoot, newFileName);
                    if ("nbproject/project.xml".equals(entry.getName())) {
                        // Special handling for setting name of Ant-based projects; customize as needed:
                        filterProjectXML(fo, str, projectRoot.getName());
                    } else
                    {
                        String ext = getFileExtension(newFileName).toLowerCase();
                        if(newFileName.equals("Makefile") ||
                           ext.equals(".cpp") ||
                           ext.equals(".h") ||
                           ext.equals(".sh") ||
                           ext.equals(".xml") ||
                           ext.equals(".config"))
                        {
                            parseFile(fo, str, newFileName, projectRoot.getName(), codeSafeProjectName);
                        }else
                        {
                            writeFile(str, fo);
                        }
                        
                        if(ext.equals(".sh"))
                        {
                            //Can we make it executable?
                            File f = FileUtil.toFile(fo);
                            if(f.exists())
                            {
                                f.setExecutable(true);
                            }
                        }
                    }
                }
            }
            
        } finally {
            source.close();
        }
    }

    private static void writeFile(ZipInputStream str, FileObject fo) throws IOException {
        OutputStream out = fo.getOutputStream();
        try {
            FileUtil.copy(str, out);
        } finally {
            out.close();
        }
    }

    private static void filterProjectXML(FileObject fo, ZipInputStream str, String name) throws IOException {
        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            FileUtil.copy(str, baos);
            Document doc = XMLUtil.parse(new InputSource(new ByteArrayInputStream(baos.toByteArray())), false, false, null, null);
            NodeList nl = doc.getDocumentElement().getElementsByTagName("name");
            if (nl != null) {
                for (int i = 0; i < nl.getLength(); i++) {
                    Element el = (Element) nl.item(i);
                    if (el.getParentNode() != null && "data".equals(el.getParentNode().getNodeName())) {
                        NodeList nl2 = el.getChildNodes();
                        if (nl2.getLength() > 0) {
                            nl2.item(0).setNodeValue(name);
                        }
                        break;
                    }
                }
            }
            OutputStream out = fo.getOutputStream();
            try {
                XMLUtil.write(doc, out, "UTF-8");
            } finally {
                out.close();
            }
        } catch (Exception ex) {
            Exceptions.printStackTrace(ex);
            writeFile(str, fo);
        }

    }

    private static void parseFile(FileObject fo, ZipInputStream str, String newFileName, String projectName, String codeSafeProjectName) throws IOException {
        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            FileUtil.copy(str, baos);
            
            String wholeFile= new String(baos.toByteArray(),"UTF-8");

            wholeFile = wholeFile.replaceAll("MyProjectApplication",codeSafeProjectName+"Application");
            wholeFile = wholeFile.replaceAll("MyProject",codeSafeProjectName);
            wholeFile = wholeFile.replaceAll("MyTitle",projectName);
            
            //These satisfy the InitialiseFiles.sh script for Android
            wholeFile = wholeFile.replaceAll("MyApplication",codeSafeProjectName);
            wholeFile = wholeFile.replaceAll("My Application",projectName);
            wholeFile = wholeFile.replaceAll("myapplication",codeSafeProjectName.toLowerCase());
            wholeFile = wholeFile.replaceAll("echoapplicationproject",codeSafeProjectName.toLowerCase());
            wholeFile = wholeFile.replaceAll("Echo.config",codeSafeProjectName+".config");
            
            OutputStream out = fo.getOutputStream();
            
            try {
                out.write(wholeFile.getBytes(Charset.forName("UTF-8")));
            } finally {
                out.close();
            }
        } catch (Exception ex) {
            Exceptions.printStackTrace(ex);
            writeFile(str, fo);
        }

    }
    
}
