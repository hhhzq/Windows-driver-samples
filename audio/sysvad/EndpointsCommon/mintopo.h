
/*++

Copyright (c) Microsoft Corporation All Rights Reserved

Module Name:

    minitopo.h

Abstract:

    Declaration of topology miniport.

--*/

#ifndef _SYSVAD_MINTOPO_H_
#define _SYSVAD_MINTOPO_H_

#include "basetopo.h"

//=============================================================================
// Classes
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// CMiniportTopology 
//   

class CMiniportTopology : 
    public CMiniportTopologySYSVAD,
    public IMiniportTopology,
    public CUnknown
{
  private:
    eDeviceType             m_DeviceType;
    union {
        PVOID               m_DeviceContext;
#if defined(SYSVAD_BTH_BYPASS)
        PSIDEBANDDEVICECOMMON m_pSidebandDevice;
#endif // defined(SYSVAD_BTH_BYPASS)
    };

public:
    DECLARE_STD_UNKNOWN();
    CMiniportTopology
    (
        _In_opt_    PUNKNOWN                UnknownOuter,
        _In_        PCFILTER_DESCRIPTOR    *FilterDesc,
        _In_        USHORT                  DeviceMaxChannels,
        _In_        eDeviceType             DeviceType, 
        _In_opt_    PVOID                   DeviceContext
    )
    : CUnknown(UnknownOuter),
      CMiniportTopologySYSVAD(FilterDesc, DeviceMaxChannels),
      m_DeviceType(DeviceType),
      m_DeviceContext(DeviceContext)
    {
#if defined(SYSVAD_BTH_BYPASS)
        if (IsSidebandDevice())
        {
            if (m_pSidebandDevice != NULL)
            {
                // This ref is released on dtor.
                m_pSidebandDevice->AddRef(); // strong ref.
            }
        }
#endif // defined(SYSVAD_BTH_BYPASS)
    }

    ~CMiniportTopology();

    IMP_IMiniportTopology;

    NTSTATUS PropertyHandlerJackDescription
    (
        _In_        PPCPROPERTY_REQUEST                      PropertyRequest,
        _In_        ULONG                                    cJackDescriptions,
        _In_reads_(cJackDescriptions) PKSJACK_DESCRIPTION *  JackDescriptions
    );

    NTSTATUS PropertyHandlerJackDescription2
    ( 
        _In_        PPCPROPERTY_REQUEST                      PropertyRequest,
        _In_        ULONG                                    cJackDescriptions,
        _In_reads_(cJackDescriptions) PKSJACK_DESCRIPTION *  JackDescriptions,
        _In_        DWORD                                    JackCapabilities
    );
    
#if defined(SYSVAD_BTH_BYPASS)
    BOOL IsSidebandDevice()
    {
        return (m_DeviceType == eBthHfpMicDevice ||
                m_DeviceType == eBthHfpSpeakerDevice) ? TRUE : FALSE;
    }

    // Returns a weak ref to the Bluetooth HFP device.
    PSIDEBANDDEVICECOMMON GetSidebandDevice() 
    {
        PSIDEBANDDEVICECOMMON sidebandDevice = NULL;
        
        if (IsSidebandDevice())
        {
            if (m_pSidebandDevice != NULL)
            {
                sidebandDevice = m_pSidebandDevice;
            }
        }

        return sidebandDevice;
    }

    static
    VOID
    EvtSpeakerVolumeHandler
    (
        _In_opt_    PVOID   Context
    );

    static
    VOID
    EvtSpeakerConnectionStatusHandler
    (
        _In_opt_    PVOID   Context
    );
    
    static
    VOID
    EvtMicVolumeHandler
    (
        _In_opt_    PVOID   Context
    );

    static
    VOID
    EvtMicConnectionStatusHandler
    (
        _In_opt_    PVOID   Context
    );
#endif // defined(SYSVAD_BTH_BYPASS)

    PVOID GetDeviceContext() { return m_DeviceContext;  }

    friend NTSTATUS PropertyHandler_BthHfpVolumeLevel_BasicSupport(
        _In_ PPCPROPERTY_REQUEST      PropertyRequest);
    friend NTSTATUS PropertyHandler_BthHfpJackContainerId(
        _In_ PPCPROPERTY_REQUEST      PropertyRequest,
        _In_ ULONG                    cJackDescriptions,
        _In_reads_(cJackDescriptions) PKSJACK_DESCRIPTION * JackDescriptions);
    friend NTSTATUS PropertyHandler_BthHfpMicVolumeLevel(
        _In_ PPCPROPERTY_REQUEST      PropertyRequest);
    friend NTSTATUS PropertyHandler_BthHfpSpeakerVolumeLevel(
        _In_ PPCPROPERTY_REQUEST      PropertyRequest);
};

typedef CMiniportTopology *PCMiniportTopology;

NTSTATUS CMiniportTopology_EventHandler_JackState
(
    _In_  PPCEVENT_REQUEST EventRequest
);

#endif // _SYSVAD_MINTOPO_H_


